#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>
#include <cocos2d.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <string>

using namespace geode::prelude;

namespace helpify {

static std::string lower(std::string text) {
    for (auto& c : text)
        c = static_cast<char>(
            std::tolower(static_cast<unsigned char>(c))
        );

    return text;
}

static bool containsSpecialName(std::string const& text) {
    auto message = lower(text);

    static constexpr std::array names = {
        "garfield",
        "onyx",
        "adrian",
        "alex",
        "kyaree",
        "aj"
    };

    for (auto name : names) {
        if (message.find(name) != std::string::npos)
            return true;
    }

    return false;
}

static void playDing() {
    auto path = (Mod::get()->getResourcesDir() / "ding.ogg").string();

    FMODAudioEngine::sharedEngine()->playEffect(path.c_str());
}

static std::string assistantName() {
    auto name =
        Mod::get()->getSettingValue<std::string>("assistant-name");

    if (name.empty())
        return "Helpify";

    return name;
}

static std::string replyTo(std::string const& message) {
    auto text = lower(message);

    if (text.empty())
        return "Type something and I'll help with your GD level.";

    if (
        text.find("theme") != std::string::npos ||
        text.find("vibe") != std::string::npos
    ) {
        return "Try a dying futuristic city: broken towers, dark fog, "
               "flickering lights, and a world slowly losing power.";
    }

    if (text.find("name") != std::string::npos) {
        return "Try: Nullspace, Afterglow, Fracture, Eventide, "
               "Blackout, Last Light, or Oblivion.";
    }

    if (
        text.find("gameplay") != std::string::npos ||
        text.find("game play") != std::string::npos
    ) {
        return "Build each section around one main mechanic, then "
               "gradually increase its difficulty. Use short transitions "
               "before major drops.";
    }

    if (
        text.find("decor") != std::string::npos ||
        text.find("decoration") != std::string::npos
    ) {
        return "Layer silhouettes, background structures, foreground "
               "objects, pulses, particles, and small moving details. "
               "Don't give every object the same visual importance.";
    }

    if (text.find("color") != std::string::npos) {
        return "For a dark level, use black or charcoal with one main "
               "accent color. Make the accent brighter during important "
               "drops or gameplay moments.";
    }

    if (
        text.find("song") != std::string::npos ||
        text.find("music") != std::string::npos
    ) {
        return "Tell me the mood, BPM, or style you want and I can help "
               "design the level's sync and atmosphere around it.";
    }

    if (text.find("boss") != std::string::npos) {
        return "Make the boss part of the environment. Its attacks can "
               "change the background, alter gameplay, trigger effects, "
               "and introduce new patterns.";
    }

    if (
        text.find("lore") != std::string::npos ||
        text.find("story") != std::string::npos
    ) {
        return "Give the level a simple story arc: discovery, escalation, "
               "collapse, and a final reveal. Let the decoration tell the "
               "story instead of relying only on text.";
    }

    if (
        text.find("transition") != std::string::npos ||
        text.find("effect") != std::string::npos
    ) {
        return "Try using flashes, camera movement, object pulses, "
               "color changes, and short movement sequences to make "
               "transitions feel connected to the song.";
    }

    if (text.find("difficulty") != std::string::npos) {
        return "Pick one core difficulty goal and make the gameplay "
               "consistent with it. For a harder level, increase timing "
               "precision and pattern complexity instead of just adding "
               "more objects.";
    }

    if (
        text.find("hello") != std::string::npos ||
        text.find("hi") != std::string::npos ||
        text.find("hey") != std::string::npos
    ) {
        return "Yo! What are we cooking? Theme, gameplay, decoration, "
               "names, songs, lore, bosses, or something else?";
    }

    return "I can help with themes, level concepts, names, gameplay, "
           "decoration, colors, songs, bosses, lore, transitions, "
           "effects, and difficulty ideas.";
}


// ============================================================
// CHAT WINDOW
// ============================================================

class HelpifyChat : public CCLayer {

protected:

    float m_width = 420.f;
    float m_height = 290.f;

    bool m_dragging = false;
    bool m_resizing = false;

    CCPoint m_startTouch;
    CCPoint m_startPosition;

    float m_startWidth = 0.f;
    float m_startHeight = 0.f;

    CCLayerColor* m_background = nullptr;

    CCLabelBMFont* m_title = nullptr;
    CCLabelBMFont* m_reply = nullptr;

    TextInput* m_input = nullptr;

    CCMenu* m_menu = nullptr;


    bool init() {

        if (!CCLayer::init())
            return false;

        auto win =
            CCDirector::sharedDirector()->getWinSize();

        this->setContentSize(
            CCSize(m_width, m_height)
        );

        this->setPosition(
            std::max(
                10.f,
                win.width / 2.f - m_width / 2.f
            ),
            std::max(
                10.f,
                win.height / 2.f - m_height / 2.f
            )
        );

        rebuild();

        return true;
    }


    void registerWithTouchDispatcher() override {

        CCTouchDispatcher::sharedDispatcher()
            ->addTargetedDelegate(
                this,
                -500,
                true
            );
    }


    void rebuild() {

        if (m_background) {

            m_background->removeFromParentAndCleanup(
                true
            );

            m_background = nullptr;
            m_input = nullptr;
            m_menu = nullptr;
        }


        m_background =
            CCLayerColor::create(
                ccc4(
                    22,
                    22,
                    27,
                    248
                ),
                m_width,
                m_height
            );

        this->addChild(
            m_background,
            0
        );


        // HEADER

        auto header =
            CCLayerColor::create(
                ccc4(
                    40,
                    40,
                    48,
                    255
                ),
                m_width,
                42.f
            );

        header->setPosition(
            0.f,
            m_height - 42.f
        );

        m_background->addChild(
            header
        );


        m_title =
            CCLabelBMFont::create(
                assistantName().c_str(),
                "bigFont.fnt"
            );

        m_title->setScale(
            0.48f
        );

        m_title->setAnchorPoint(
            CCPoint(0.f, 0.5f)
        );

        m_title->setPosition(
            14.f,
            21.f
        );

        header->addChild(
            m_title
        );


        // MENU

        m_menu =
            CCMenu::create();

        m_menu->setPosition(
            CCPoint(0.f, 0.f)
        );

        m_background->addChild(
            m_menu,
            5
        );


        // CLOSE BUTTON

        auto closeSprite =
            ButtonSprite::create(
                "X",
                28,
                true,
                "goldFont.fnt",
                "GJ_button_01.png",
                1.f,
                1.f
            );

        auto close =
            CCMenuItemSpriteExtra::create(
                closeSprite,
                this,
                menu_selector(
                    HelpifyChat::onClose
                )
            );

        close->setPosition(
            m_width - 22.f,
            m_height - 21.f
        );

        m_menu->addChild(
            close
        );


        // REPLY

        m_reply =
            CCLabelBMFont::create(
                "Ask me about your level.",
                "chatFont.fnt"
            );

        m_reply->setScale(
            0.55f
        );

        m_reply->setWidth(
            std::max(
                100.f,
                m_width - 42.f
            )
        );

        m_reply->setLineBreakWithoutSpace(
            true
        );

        m_reply->setAnchorPoint(
            CCPoint(0.f, 1.f)
        );

        m_reply->setPosition(
            20.f,
            m_height - 65.f
        );

        m_background->addChild(
            m_reply,
            2
        );


        // TEXT INPUT

        float inputWidth =
            std::max(
                150.f,
                m_width - 120.f
            );

        auto input =
            TextInput::create(
                inputWidth,
                "Ask Helpify...",
                "bigFont.fnt"
            );

        input->setMaxCharCount(
            180
        );

        input->setPosition(
            20.f + inputWidth / 2.f,
            28.f
        );

        m_input = input;

        m_background->addChild(
            input,
            4
        );


        // SEND BUTTON

        auto sendSprite =
            ButtonSprite::create(
                "SEND",
                46,
                true,
                "goldFont.fnt",
                "GJ_button_01.png",
                1.f,
                1.f
            );

        auto send =
            CCMenuItemSpriteExtra::create(
                sendSprite,
                this,
                menu_selector(
                    HelpifyChat::onSend
                )
            );

        send->setPosition(
            m_width - 46.f,
            28.f
        );

        m_menu->addChild(
            send
        );


        // RESIZE CORNER

        auto corner =
            CCLayerColor::create(
                ccc4(
                    255,
                    255,
                    255,
                    120
                ),
                16.f,
                16.f
            );

        corner->setPosition(
            m_width - 16.f,
            0.f
        );

        corner->setID(
            "resize-corner"
        );

        m_background->addChild(
            corner,
            6
        );
    }


    void onClose(CCObject*) {

        this->removeFromParentAndCleanup(
            true
        );
    }


    void onSend(CCObject*) {

        if (!m_input)
            return;

        auto message =
            std::string(
                m_input->getString()
            );

        if (message.empty())
            return;


        // Special-name ding

        if (containsSpecialName(message))
            playDing();


        auto answer =
            replyTo(message);


        std::string output =
            assistantName() +
            ": " +
            answer;


        m_reply->setString(
            output.c_str()
        );

        m_reply->setWidth(
            std::max(
                100.f,
                m_width - 42.f
            )
        );


        m_input->setString(
            "",
            false
        );
    }


    bool inHeader(CCPoint p) const {

        return
            p.x >= 0.f &&
            p.x <= m_width &&
            p.y >= m_height - 42.f &&
            p.y <= m_height;
    }


    bool inResizeCorner(CCPoint p) const {

        return
            p.x >= m_width - 32.f &&
            p.x <= m_width + 4.f &&
            p.y <= 32.f;
    }


    bool ccTouchBegan(
        CCTouch* touch,
        CCEvent*
    ) override {

        auto p =
            this->convertToNodeSpace(
                touch->getLocation()
            );


        if (inResizeCorner(p)) {

            m_resizing = true;
            m_dragging = false;

            m_startTouch =
                touch->getLocation();

            m_startWidth =
                m_width;

            m_startHeight =
                m_height;

            return true;
        }


        if (inHeader(p)) {

            m_dragging = true;
            m_resizing = false;

            m_startTouch =
                touch->getLocation();

            m_startPosition =
                this->getPosition();

            return true;
        }


        return false;
    }


    void ccTouchMoved(
        CCTouch* touch,
        CCEvent*
    ) override {

        auto now =
            touch->getLocation();


        if (m_dragging) {

            auto dx =
                now.x -
                m_startTouch.x;

            auto dy =
                now.y -
                m_startTouch.y;


            this->setPosition(
                CCPoint(
                    m_startPosition.x + dx,
                    m_startPosition.y + dy
                )
            );
        }


        else if (m_resizing) {

            auto dx =
                now.x -
                m_startTouch.x;

            auto dy =
                now.y -
                m_startTouch.y;


            m_width =
                std::max(
                    300.f,
                    std::min(
                        700.f,
                        m_startWidth + dx
                    )
                );


            m_height =
                std::max(
                    220.f,
                    std::min(
                        500.f,
                        m_startHeight - dy
                    )
                );


            this->setContentSize(
                CCSize(
                    m_width,
                    m_height
                )
            );


            rebuild();
        }
    }


    void ccTouchEnded(
        CCTouch*,
        CCEvent*
    ) override {

        m_dragging = false;
        m_resizing = false;
    }


    void ccTouchCancelled(
        CCTouch*,
        CCEvent*
    ) override {

        m_dragging = false;
        m_resizing = false;
    }


public:

    static HelpifyChat* create() {

        auto ret =
            new HelpifyChat();

        if (
            ret &&
            ret->init()
        ) {

            ret->autorelease();

            return ret;
        }

        delete ret;

        return nullptr;
    }
};


// ============================================================
// FLOATING ICON
// ============================================================

class HelpifyIconLayer : public CCLayer {

protected:

    bool m_dragging = false;

    CCPoint m_startTouch;
    CCPoint m_startPosition;

    CCSprite* m_icon = nullptr;


    bool init() {

        if (!CCLayer::init())
            return false;


        auto win =
            CCDirector::sharedDirector()->getWinSize();


        auto scale =
            static_cast<float>(
                Mod::get()->getSettingValue<double>(
                    "icon-scale"
                )
            );


        auto icon =
            CCSprite::create(
                "HelpifyIcon.png"_spr
            );


        if (!icon)
            return false;


        m_icon = icon;


        icon->setScale(
            0.16f * scale
        );


        this->addChild(
            icon
        );


        this->setContentSize(
            icon->getContentSize() *
            icon->getScale()
        );


        this->setPosition(
            win.width - 65.f,
            win.height - 65.f
        );


        return true;
    }


    void registerWithTouchDispatcher() override {

        CCTouchDispatcher::sharedDispatcher()
            ->addTargetedDelegate(
                this,
                -1000,
                true
            );
    }


    bool ccTouchBegan(
        CCTouch* touch,
        CCEvent*
    ) override {

        auto local =
            this->convertToNodeSpace(
                touch->getLocation()
            );


        auto size =
            this->getContentSize();


        if (
            local.x < 0.f ||
            local.y < 0.f ||
            local.x > size.width ||
            local.y > size.height
        ) {
            return false;
        }


        m_startTouch =
            touch->getLocation();


        m_startPosition =
            this->getPosition();


        m_dragging = false;


        return true;
    }


    void ccTouchMoved(
        CCTouch* touch,
        CCEvent*
    ) override {

        auto now =
            touch->getLocation();


        auto dx =
            now.x -
            m_startTouch.x;

        auto dy =
            now.y -
            m_startTouch.y;


        if (
            !m_dragging &&
            (
                std::abs(dx) > 6.f ||
                std::abs(dy) > 6.f
            )
        ) {
            m_dragging = true;
        }


        if (!m_dragging)
            return;


        auto win =
            CCDirector::sharedDirector()->getWinSize();


        auto halfW =
            this->getContentSize().width /
            2.f;

        auto halfH =
            this->getContentSize().height /
            2.f;


        auto x =
            std::max(
                halfW,
                std::min(
                    win.width - halfW,
                    m_startPosition.x + dx
                )
            );


        auto y =
            std::max(
                halfH,
                std::min(
                    win.height - halfH,
                    m_startPosition.y + dy
                )
            );


        this->setPosition(
            CCPoint(x, y)
        );
    }


    void ccTouchEnded(
        CCTouch*,
        CCEvent*
    ) override {

        if (!m_dragging) {

            auto chat =
                HelpifyChat::create();

            if (chat) {

                this->getParent()
                    ->addChild(
                        chat,
                        2000
                    );
            }
        }


        m_dragging = false;
    }


    void ccTouchCancelled(
        CCTouch*,
        CCEvent*
    ) override {

        m_dragging = false;
    }


public:

    static HelpifyIconLayer* create() {

        auto ret =
            new HelpifyIconLayer();

        if (
            ret &&
            ret->init()
        ) {

            ret->autorelease();

            return ret;
        }

        delete ret;

        return nullptr;
    }
};


// ============================================================
// GEOMETRY DASH MENU LAYER
// ============================================================

class $modify(
    HelpifyMenuLayer,
    MenuLayer
) {

    bool init() {

        if (!MenuLayer::init())
            return false;


        auto helpify =
            HelpifyIconLayer::create();


        if (helpify) {

            this->addChild(
                helpify,
                999
            );
        }


        return true;
    }
};


// ============================================================
// MOD LOADED
// ============================================================

$on_mod(Loaded) {

    log::info(
        "Helpify loaded successfully!"
    );
}

}
