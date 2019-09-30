#pragma once

#include "../BaseState.hpp"
#include "../EntityManager.hpp"

namespace Components { struct UIButton; }

namespace States
{

class MenuState : public BaseState
{
public:
    MenuState();
    ~MenuState();

    virtual void init() override;
    virtual void handleEvent(const sf::Event& aEvent) override;
    virtual void update(const float aDt) override;
    virtual void fixedUpdate(const float aDt) override;
    virtual void render(const float aAlpha) override;

private:
    Components::UIButton& addButton(const std::string& aTitle);

    EntityManager m_backgroundManager,
                  m_foregroundManager;
};

}