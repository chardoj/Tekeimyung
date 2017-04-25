/*
** Author : Simon AMBROISE
*/

#include <cmath>

#include <Engine/Components.hh>
#include <Engine/EntityFactory.hpp>
#include <Engine/Graphics/Camera.hpp>
#include <Engine/Graphics/Renderer.hpp>
#include <Engine/Physics/Physics.hpp>
#include <Engine/Utils/Exception.hpp>

#include <Game/Scripts/GameManager.hpp>
#include <Game/Scripts/Projectile.hpp>
#include <Game/Scripts/Player.hpp>
#include <Game/Scripts/TutoManagerMessage.hpp>
#include <Game/Scripts/WaveManager.hpp>

#include <Game/Weapons/DefaultWeapon.hpp>
#include <Game/Weapons/TeslaWeapon.hpp>
#include <Game/Weapons/LaserWeapon.hpp>

void Player::death()
{
    this->Destroy();
}

void Player::start()
{
    this->_weapons.push_back(new DefaultWeapon());
    //this->_weapons.push_back(new TeslaWeapon());
    this->_weapons.push_back(new LaserWeapon());

    this->_levelUpReward[2] = std::make_pair<std::string, double>("FireRate", -25.0 / 100.0);
    this->_levelUpReward[3] = std::make_pair<std::string, double>("FireRate", -25.0 / 100.0);

    this->_attributes["Speed"] = new Attribute(80.0f);

    setHealth(200);
    setMaxHealth(200);

    this->_transform = this->getComponent<sTransformComponent>();
    this->_render = this->getComponent<sRenderComponent>();
    this->_rigidBody = this->getComponent<sRigidBodyComponent>();
    _buildEnabled = false;

    // Get GameManager
    {
        auto em = EntityFactory::getBindedEntityManager();
        Entity* gameManager = em->getEntityByTag("GameManager");
        if (!gameManager)
        {
            LOG_WARN("Can't find entity with GameManager tag");
            return;
        }

        auto scriptComponent = gameManager->getComponent<sScriptComponent>();

        if (!scriptComponent)
        {
            LOG_WARN("Can't find scriptComponent on GameManager entity");
            return;
        }

        _gameManager = scriptComponent->getScript<GameManager>("GameManager");
        _waveManager = scriptComponent->getScript<WaveManager>("WaveManager");
    }

    updateWeaponMaterial();
}

void Player::update(float dt)
{
    this->updateDirection();
    this->movement(dt);
    this->handleShoot(dt);
    this->changeWeapon();
}

void Player::changeWeapon()
{
    auto &&scroll = mouse.getScroll();
    static double lastOffset = scroll.yOffset;

    if (lastOffset != scroll.yOffset &&
        !keyboard.isPressed(Keyboard::eKey::LEFT_CONTROL))
    {
        this->_weapons[this->_actualWeapon]->clean();

        if (this->mouse.getScroll().yOffset < 0)
            this->_actualWeapon++;
        else
            this->_actualWeapon--;

        if (this->_actualWeapon >= (int)this->_weapons.size())
            this->_actualWeapon = 0;
        else if (this->_actualWeapon < 0)
            this->_actualWeapon = static_cast<int>(this->_weapons.size() - 1);

        updateWeaponMaterial();
        TutoManagerMessage::getInstance()->sendMessage(eTutoState::CHANGE_WEAPON);
    }

    lastOffset = scroll.yOffset;
}

void Player::updateDirection()
{
    Cursor& cursor = GameWindow::getInstance()->getMouse().getCursor();
    Camera* camera = Renderer::getInstance()->getCurrentCamera();

    if (!camera)
        return;

    Ray ray = camera->screenPosToRay((float)cursor.getX(), (float)cursor.getY());
    float hitDistance;

    if (Physics::raycastPlane(ray, { 0.0f, 1.0f, 0.0f }, { 0.0f, _transform->getPos().y, 0.0f }, hitDistance))
    {
        glm::vec3 target = ray.getPoint(hitDistance);
        _direction = glm::normalize(target - _transform->getPos());
    }
}

void Player::movement(float elapsedTime)
{
    // update player rotation
    {
        float rotation = glm::degrees(std::atan2(_direction.x, _direction.z));
        _transform->setRotation(glm::vec3(_transform->getRotation().x, rotation, _transform->getRotation().z));
    }

    if (!_rigidBody)
    {
        return;
    }

    _rigidBody->velocity = glm::vec3(0.0f);

    // update player position
    {
        if (KB_P(Keyboard::eKey::S))
        {
            _rigidBody->velocity += glm::vec3(this->_attributes["Speed"]->getFinalValue(), 0.0f, this->_attributes["Speed"]->getFinalValue());
            TutoManagerMessage::getInstance()->sendMessage(eTutoState::MOVE);
        }
        if (KB_P(Keyboard::eKey::Z))
        {
            _rigidBody->velocity += glm::vec3(-this->_attributes["Speed"]->getFinalValue(), 0.0f, -this->_attributes["Speed"]->getFinalValue());
            TutoManagerMessage::getInstance()->sendMessage(eTutoState::MOVE);
        }
        if (KB_P(Keyboard::eKey::Q))
        {
            _rigidBody->velocity += glm::vec3(-this->_attributes["Speed"]->getFinalValue(), 0.0f, this->_attributes["Speed"]->getFinalValue());
            TutoManagerMessage::getInstance()->sendMessage(eTutoState::MOVE);
        }
        if (KB_P(Keyboard::eKey::D))
        {
            _rigidBody->velocity += glm::vec3(this->_attributes["Speed"]->getFinalValue(), 0.0f, -this->_attributes["Speed"]->getFinalValue());
            TutoManagerMessage::getInstance()->sendMessage(eTutoState::MOVE);
        }
    }

#if defined(ENGINE_DEBUG)
    if (this->keyboard.getStateMap()[Keyboard::eKey::L] == Keyboard::eKeyState::KEY_RELEASED)
    {
        this->_weapons[this->_actualWeapon]->levelUp();
    }
#endif
}

void Player::handleShoot(float dt)
{
    this->_elapsedTime += dt;

    if (this->_canShoot && this->_elapsedTime > this->_weapons[this->_actualWeapon]->getAttribute("FireRate"))
    {
        if (mouse.isPressed(Mouse::eButton::MOUSE_BUTTON_1))
        {
            this->_weapons[this->_actualWeapon]->fire(this, this->_transform, this->_render, this->_direction);
            this->_elapsedTime = 0;
            TutoManagerMessage::getInstance()->sendMessage(eTutoState::SHOOT);
        }
        else
            this->_weapons[this->_actualWeapon]->clean();
    }
}

void Player::onHoverEnter()
{
}

void Player::onHoverExit()
{
}

void Player::onCollisionEnter(Entity* entity)
{
}

void Player::onCollisionExit(Entity* entity)
{
}

void Player::addExperience(int xp)
{
    this->_experience += xp;

    if (this->_experience > this->_nextLevelUp)
        this->levelUp();
}

void Player::updateWeaponMaterial()
{
    Material* weaponMaterial = this->_weapons[this->_actualWeapon]->getMaterial();

    if (!weaponMaterial)
    {
        EXCEPT(InternalErrorException, "Missing material for weapon %d", this->_actualWeapon);
    }

    _render->getModelInstance()->setMaterial(weaponMaterial);
}

void Player::levelUp()
{
    this->_nextLevelUp += 250;

    this->_level++;

    std::pair<std::string, double> reward = this->_levelUpReward[this->_level];

    //this->_attributes[reward.first]->addModifier(Modifier(reward.second));
}

void Player::setCanShoot(bool canShoot)
{
    this->_canShoot = canShoot;
}

bool Player::updateEditor()
{
    bool changed = false;

    changed |= ImGui::InputFloat("Speed", &_speed, 10.0f, ImGuiInputTextFlags_AllowTabInput);

    int health = getMaxHealth();
    if (ImGui::InputInt("Health", &health, 10, ImGuiInputTextFlags_AllowTabInput))
    {
        if (health < 0)
            health = 0;
        setMaxHealth(health);
        changed = true;
    }

    if (ImGui::InputInt("Experience earned", &this->_experienceEarned, 10, ImGuiInputTextFlags_AllowTabInput))
    {
        if (this->_experienceEarned < 0)
            this->_experienceEarned = 0;
        changed = true;
    }

    return (changed);
}

JsonValue Player::saveToJson()
{
    JsonValue json;

    json.setFloat("speed", _speed);
    json.setUInt("health", getMaxHealth());
    json.setUInt("experience_earned", this->_experienceEarned);

    return (json);
}

void    Player::loadFromJson(const JsonValue& json)
{
    _speed = json.getFloat("speed", 50.0f);
    setMaxHealth(json.getUInt("health", 150));
    this->_experienceEarned = json.getUInt("experience_earned", 20);
}