#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class HelpifyChat : public CCLayer {
protected:
    bool m_dragging = false;
    CCPoint m_startTouch;
    CCPoint m_startPosition;

    bool init() {
        if (!CCLayer::init())
            return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto background = CCLayerColor::create(
            ccc4(25, 25, 30, 245),
            360.f,
            250.f
        );

        background->setPosition(
            winSize.width / 2.f - 180.f,
            winSize.height / 2.f - 125.f
        );

        this->addChild(background);

        auto title = CCLabelBMFont::create(
            "HELPIFY",
            "bigFont.fnt"
        );

        title->setScale(0.55f);
        title->setPosition(180.f, 220.f);
        background->addChild(title);

        auto message = CCLabelBMFont::create(
            "Ask me anything!",
            "goldFont.fnt"
        );

        message->setScale(0.55f);
        message->setPosition(180.f, 150.f);
        background->addChild(message);

        auto closeSprite = ButtonSprite::create(
            "X",
            30,
            true,
            "goldFont.fnt",
            "GJ_button_01.png",
            1.f,
            1.f
        );

        auto closeButton = CCMenuItemSpriteExtra::create(
            closeSprite,
            this,
            menu_selector(HelpifyChat::onClose)
        );

        auto menu = CCMenu::create();
        menu->setPosition(330.f, 220.f);
        menu->addChild(closeButton);
        background->addChild(menu);

        return true;
    }

    void onClose(CCObject*) {
        this->removeFromParentAndCleanup(true);
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent*) override {
        auto location = touch->getLocation();

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        float left = winSize.width / 2.f - 180.f;
        float right = winSize.width / 2.f + 180.f;
        float top = winSize.height / 2.f + 125.f;

        if (
            location.x >= left &&
            location.x <= right &&
            location.y >= top - 45.f &&
            location.y <= top
        ) {
            m_startTouch = location;
            m_startPosition = this->getPosition();
            m_dragging = true;
            return true;
        }

        return false;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent*) override {
        if (!m_dragging)
            return;

        auto location = touch->getLocation();

        float dx = location.x - m_startTouch.x;
        float dy = location.y - m_startTouch.y;

        CCPoint newPosition(
            m_startPosition.x + dx,
            m_startPosition.y + dy
        );

        this->setPosition(newPosition);
    }

    void ccTouchEnded(CCTouch*, CCEvent*) override {
        m_dragging = false;
    }

public:
    static HelpifyChat* create() {
        auto ret = new HelpifyChat();

        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};


class HelpifyIconLayer : public CCLayer {
protected:
    bool m_dragging = false;
    CCPoint m_startTouch;
    CCPoint m_startPosition;

    bool init() {
        if (!CCLayer::init())
            return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto icon = CCSprite::create("HelpifyIcon.png"_spr);

        if (!icon)
            return false;

        icon->setScale(0.8f);
        this->addChild(icon);

        this->setContentSize(icon->getContentSize());

        this->setPosition(
            winSize.width - 70.f,
            winSize.height - 70.f
        );

        return true;
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent*) override {
        auto location = touch->getLocation();
        auto local = this->convertToNodeSpace(location);

        auto size = this->getContentSize();

        if (
            local.x < 0 ||
            local.y < 0 ||
            local.x > size.width ||
            local.y > size.height
        )
            return false;

        m_startTouch = location;
        m_startPosition = this->getPosition();
        m_dragging = false;

        return true;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent*) override {
        auto location = touch->getLocation();

        float dx = location.x - m_startTouch.x;
        float dy = location.y - m_startTouch.y;

        if (std::abs(dx) > 8.f || std::abs(dy) > 8.f)
            m_dragging = true;

        if (!m_dragging)
            return;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        CCPoint newPosition(
            m_startPosition.x + dx,
            m_startPosition.y + dy
        );

        newPosition.x = std::max(
            35.f,
            std::min(newPosition.x, winSize.width - 35.f)
        );

        newPosition.y = std::max(
            35.f,
            std::min(newPosition.y, winSize.height - 35.f)
        );

        this->setPosition(newPosition);
    }

    void ccTouchEnded(CCTouch*, CCEvent*) override {
        if (!m_dragging) {
            auto chat = HelpifyChat::create();

            if (chat)
                this->getParent()->addChild(chat, 1000);
        }

        m_dragging = false;
    }

public:
    static HelpifyIconLayer* create() {
        auto ret = new HelpifyIconLayer();

        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};


class $modify(HelpifyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        auto helpify = HelpifyIconLayer::create();

        if (helpify)
            this->addChild(helpify, 100);

        return true;
    }
};


$on_mod(Loaded) {
    log::info("Helpify loaded successfully!");
}
