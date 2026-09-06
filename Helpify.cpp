#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <cocos2d.h>
#include <array>
#include <random>
#include <string>
#include <vector>

using namespace geode::prelude;

namespace helpify {
    struct Suggestion {
        std::string name;
        std::string idea;
    };

    static Suggestion generateSuggestion() {
        static std::mt19937 rng(std::random_device{}());

        static constexpr std::array names = {
            "Afterglow", "Nullspace", "Starfall", "Fracture", "Eventide",
            "Blackout", "Parallax", "Singularity", "Ashen", "Voidline",
            "Last Light", "Terminal", "Dead Signal", "Redshift", "Cataclysm",
            "Echofall", "Fading Point", "Hollow Sky", "Static Bloom", "Final Dawn"
        };

        static constexpr std::array ideas = {
            "A collapsing digital city slowly loses its colors while the gameplay gets faster.",
            "An abandoned space station has broken gravity and distant stars in the background.",
            "A storm consumes a futuristic skyline and every major drop changes the atmosphere.",
            "A forgotten machine wakes up and rebuilds the level around the player.",
            "The world fades into darkness, interrupted by short bursts of bright energy.",
            "A huge eclipse moves across the sky and changes the decoration section by section.",
            "A corrupted dream switches between peaceful gameplay and sudden intense drops.",
            "A ruined civilization is swallowed by glowing fog as the level progresses.",
            "A giant reactor becomes increasingly unstable until the final section collapses.",
            "A frozen world cracks open to reveal a massive energy source underneath.",
            "A mysterious signal changes the gameplay style whenever a new section begins.",
            "The player escapes a collapsing dimension with increasingly aggressive transitions."
        };

        std::uniform_int_distribution<size_t> nameDist(0, names.size() - 1);
        std::uniform_int_distribution<size_t> ideaDist(0, ideas.size() - 1);
        return { names[nameDist(rng)], ideas[ideaDist(rng)] };
    }

    static std::string assistantName() {
        auto name = Mod::get()->getSettingValue<std::string>("assistant-name");
        return name.empty() ? "Helpify" : name;
    }

    class HelpifyPopup : public geode::Popup<> {
    protected:
        CCLabelBMFont* m_nameLabel = nullptr;
        CCLabelBMFont* m_ideaLabel = nullptr;

        bool init() {
            if (!Popup::init(360.f, 245.f)) return false;
            this->setTitle(assistantName());

            auto icon = CCSprite::create("HelpifyIcon.png"_spr);
            if (icon) {
                icon->setScale(0.12f);
                m_mainLayer->addChildAtPosition(icon, Anchor::Top, {0.f, -37.f});
            }

            m_nameLabel = CCLabelBMFont::create("", "goldFont.fnt");
            m_nameLabel->setScale(0.65f);
            m_nameLabel->setWidth(300.f);
            m_nameLabel->setLineBreakWithoutSpace(true);
            m_mainLayer->addChildAtPosition(m_nameLabel, Anchor::Center, {0.f, 34.f});

            m_ideaLabel = CCLabelBMFont::create("", "chatFont.fnt");
            m_ideaLabel->setScale(0.55f);
            m_ideaLabel->setWidth(310.f);
            m_ideaLabel->setLineBreakWithoutSpace(true);
            m_mainLayer->addChildAtPosition(m_ideaLabel, Anchor::Center, {0.f, -20.f});

            auto menu = CCMenu::create();
            menu->setPosition({0.f, 0.f});
            m_mainLayer->addChild(menu);

            auto generate = CCMenuItemSpriteExtra::create(
                ButtonSprite::create("Generate", "goldFont.fnt", "GJ_button_01.png", 0.72f),
                this,
                menu_selector(HelpifyPopup::onGenerate)
            );
            auto settings = CCMenuItemSpriteExtra::create(
                ButtonSprite::create("Settings", "goldFont.fnt", "GJ_button_01.png", 0.72f),
                this,
                menu_selector(HelpifyPopup::onSettings)
            );

            menu->addChild(generate);
            menu->addChild(settings);
            menu->alignItemsHorizontallyWithPadding(8.f);
            menu->setPosition({180.f, 23.f});

            this->showSuggestion(generateSuggestion());
            return true;
        }

        void showSuggestion(Suggestion const& suggestion) {
            if (m_nameLabel) {
                m_nameLabel->setString(fmt::format("Name: {}", suggestion.name).c_str());
            }
            if (m_ideaLabel) {
                m_ideaLabel->setString(fmt::format("Idea: {}", suggestion.idea).c_str());
            }
        }

        void onGenerate(CCObject*) {
            this->showSuggestion(generateSuggestion());
        }

        void onSettings(CCObject*) {
            geode::openSettingsPopup(Mod::get(), false);
        }

    public:
        static HelpifyPopup* create() {
            auto ret = new HelpifyPopup();
            if (ret->init()) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }
    };

    static void open() {
        if (auto popup = HelpifyPopup::create()) popup->show();
    }
}

class $modify(HelpifyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto win = CCDirector::sharedDirector()->getWinSize();
        auto scale = static_cast<float>(Mod::get()->getSettingValue<double>("icon-scale"));
        auto px = static_cast<float>(Mod::get()->getSettingValue<double>("icon-x"));
        auto py = static_cast<float>(Mod::get()->getSettingValue<double>("icon-y"));

        auto icon = CCSprite::create("HelpifyIcon.png"_spr);
        if (!icon) {
            log::error("HelpifyIcon.png could not be loaded");
            return true;
        }

        // The source image is a full square logo; scale it down into a compact button.
        icon->setScale(0.16f * scale);
        auto button = CCMenuItemSpriteExtra::create(
            icon,
            this,
            menu_selector(HelpifyMenuLayer::onHelpify)
        );
        button->setID("helpify-button");

        auto menu = CCMenu::create();
        menu->setID("helpify-menu");
        menu->setPosition({0.f, 0.f});
        menu->addChild(button);
        button->setPosition({win.width * px, win.height * py});
        this->addChild(menu, 999);

        return true;
    }

    void onHelpify(CCObject*) {
        helpify::open();
    }
};

$on_mod(Loaded) {
    log::info("Helpify loaded successfully");
}
