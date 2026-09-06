#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(HelpifyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        auto icon = CCSprite::create("HelpifyIcon.png"_spr);

        if (icon) {
            icon->setScale(0.8f);

            auto size = CCDirector::sharedDirector()->getWinSize();

            icon->setPosition({
                size.width * 0.08f,
                size.height * 0.15f
            });

            this->addChild(icon, 100);
        }

        return true;
    }
};

$on_mod(Loaded) {
    log::info("Helpify loaded successfully!");
}
