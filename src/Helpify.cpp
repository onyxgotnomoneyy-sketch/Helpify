#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class HelpifyLayer : public CCLayer {
protected:
    bool init() {
        if (!CCLayer::init())
            return false;

        auto bg = CCLayerColor::create(
            ccc4(20, 20, 20, 245)
        );
        this->addChild(bg);

        auto size = CCDirector::sharedDirector()->getWinSize();

        auto title = CCLabelBMFont::create(
            "HELPIFY",
            "bigFont.fnt"
        );
        title->setPosition({
            size.width / 2,
            size.height - 40
        });
        title->setScale(0.8f);
        this->addChild(title);

        auto text = CCLabelBMFont::create(
            "Hello! I'm Helpify!",
            "goldFont.fnt"
        );
        text->setPosition({
            size.width / 2,
            size.height / 2
        });
        text->setScale(0.6f);
        this->addChild(text);

        auto close = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Close"),
            this,
            menu_selector(HelpifyLayer::onClose)
        );

        auto menu = CCMenu::create();
        menu->addChild(close);
        menu->setPosition({
            size.width / 2,
            40
        });
        this->addChild(menu);

        return true;
    }

    void onClose(CCObject*) {
        this->removeFromParentAndCleanup(true);
    }

public:
    static HelpifyLayer* create() {
        auto ret = new HelpifyLayer();

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

        auto icon = CCSprite::create("HelpifyIcon.png"_spr);

        if (icon) {
            icon->setScale(0.8f);

            auto size = CCDirector::sharedDirector()->getWinSize();

            auto button = CCMenuItemSpriteExtra::create(
                icon,
                this,
                menu_selector(HelpifyMenuLayer::onHelpify)
            );

            auto menu = CCMenu::create();
            menu->addChild(button);
            menu->setPosition({
                size.width * 0.08f,
                size.height * 0.15f
            });

            this->addChild(menu, 100);
        }

        return true;
    }

    void onHelpify(CCObject*) {
        auto layer = HelpifyLayer::create();

        if (layer) {
            this->addChild(layer, 1000);
        }
    }
};

$on_mod(Loaded) {
    log::info("Helpify loaded successfully!");
}
