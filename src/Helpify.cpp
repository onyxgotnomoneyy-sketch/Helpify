#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class HelpifyChat : public CCLayer {
protected:
    CCPoint m_dragOffset;
    bool m_dragging = false;

    bool init() {
        if (!CCLayer::init())
            return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Chat background
        auto background = CCLayerColor::create(
            ccc4(25, 25, 30, 245),
            360.f,
            250.f
        );

        background->setPosition({
            winSize.width / 2.f - 180.f,
            winSize.height / 2.f - 125.f
        });

        this->addChild(background, 0);

        // Header
        auto header = CCLabelBMFont::create(
            "HELPIFY",
            "bigFont.fnt"
        );

        header->setScale(0.55f);
        header->setPosition({
            180.f,
            225.f
        });

        background->addChild(header);

        // Welcome message
        auto message = CCLabelBMFont::create(
            "Ask me anything!",
            "goldFont.fnt"
        );

        message->setScale(0.55f);
        message->setPosition({
            180.f,
            150.f
        });

        background->addChild(message);

        // Close button
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

        auto closeMenu = CCMenu::create();
        closeMenu->setPosition({
            330.f,
            220.f
        });

        closeMenu->addChild(closeButton);
        background->addChild(closeMenu);

        return true;
    }

    void onClose(CCObject*) {
        this->removeFromParentAndCleanup(true);
    }

    virtual bool ccTouchBegan(
        CCTouch* touch,
        CCEvent* event
    ) override {
        auto location = touch->getLocation();

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        float left = winSize.width / 2.f - 180.f;
        float right = winSize.width / 2.f + 180.f;
        float bottom = winSize.height / 2.f - 125.f;
        float top = winSize.height / 2.f + 125.f;

        if (
            location.x >= left &&
            location.x <= right &&
            location.y >= top - 45.f &&
            location.y <= top
        ) {
            m_dragging = true;

            m_dragOffset = {
                location.x - left,
                location.y - bottom
            };

            return true;
        }

        return false;
    }

    virtual void ccTouchMoved(
        CCTouch* touch,
        CCEvent* event
    ) override {
        if (!m_dragging)
            return;

        auto location = touch->getLocation();
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        float newLeft = location.x - m_dragOffset.x;
        float newBottom = location.y - m_dragOffset.y;

        newLeft = std::max(0.f, std::min(newLeft, winSize.width - 360.f));
        newBottom = std::max(0.f, std::min(newBottom, winSize.height - 250.f));

        this->setPosition({
            newLeft - (winSize.width / 2.f - 180.f),
            newBottom - (winSize.height / 2.f - 125.f)
        });
    }

    virtual void ccTouchEnded(
        CCTouch* touch,
        CCEvent* event
    ) override {
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
    CCPoint m_startTouch;
    CCPoint m_startPosition;
    bool m_dragging = false;

    bool init() {
        if (!CCLayer::init())
            return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto icon = CCSprite::create("HelpifyIcon.png"_spr);

        if (!icon)
            return true;

        icon->setScale(0.8f);

        this->addChild(icon);

        this->setContentSize(icon->getContentSize());

        this->setPosition({
            winSize.width - 90.f,
            winSize.height - 90.f
        });

        return true;
    }

    bool ccTouchBegan(
        CCTouch* touch,
        CCEvent* event
    ) override {
        auto location = touch->getLocation();

        auto rect = CCRectMake(
            0,
            0,
            this->getContentSize().width,
            this->getContentSize().height
        );

        auto local = this->convertToNodeSpace(location);

        if (!rect.containsPoint(local))
            return false;

        m_startTouch = location;
        m_startPosition = this->getPosition();
        m_dragging = false;

        return true;
    }

    void ccTouchMoved(
        CCTouch* touch,
        CCEvent* event
    ) override {
        auto location = touch->getLocation();

        float dx = location.x - m_startTouch.x;
        float dy = location.y - m_startTouch.y;

        if (std::abs(dx) > 8.f || std::abs(dy) > 8.f)
            m_dragging = true;

        if (!m_dragging)
            return;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto newPosition = CCPoint(
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

    void ccTouchEnded(
        CCTouch* touch,
        CCEvent* event
    ) override {
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
