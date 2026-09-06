#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>
#include <cocos2d.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <string>

using namespace geode::prelude;

namespace helpify {

// ============================================================
// HELPERS
// ============================================================

static std::string lower(std::string text) {
    for (auto& c : text) {
        c = static_cast<char>(
            std::tolower(static_cast<unsigned char>(c))
        );
    }

    return text;
}

static bool containsInsensitive(
    std::string const& text,
    std::string const& search
) {
    return lower(text).find(lower(search)) != std::string::npos;
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

    for (auto const& name : names) {
        auto pos = message.find(name);

        if (pos == std::string::npos)
            continue;

        bool leftOK =
            pos == 0 ||
            !std::isalnum(
                static_cast<unsigned char>(message[pos - 1])
            );

        auto end = pos + std::strlen(name);

        bool rightOK =
            end >= message.size() ||
            !std::isalnum(
                static_cast<unsigned char>(message[end])
            );

        if (leftOK && rightOK)
            return true;
    }

    return false;
}

static std::string assistantName() {
    auto name =
        Mod::get()->getSettingValue<std::string>(
            "assistant-name"
        );

    return name.empty() ? "Helpify" : name;
}

static void playDing() {
    auto path =
        Mod::get()->getResourcesDir() /
        "ding.wav";

    FMODAudioEngine::sharedEngine()->playEffect(
        path.string().c_str()
    );
}


// ============================================================
// LOCAL RESPONSE ENGINE
// ============================================================

static std::string replyTo(std::string const& input) {

    auto text = lower(input);

    // GREETING
    if (
        containsInsensitive(text, "hello") ||
        containsInsensitive(text, "hey") ||
        text == "hi"
    ) {
        return
            "Yo! Give me a theme, level idea, name, "
            "gameplay idea, decoration problem, song "
            "vibe, boss, lore, transition, or difficulty "
            "question.";
    }

    // THEME
    if (
        containsInsensitive(text, "theme") ||
        containsInsensitive(text, "aesthetic") ||
        containsInsensitive(text, "vibe")
    ) {
        return
            "Try a dying futuristic city: massive broken "
            "buildings, dark fog, flickering lights, "
            "floating debris, and a sky that gradually "
            "falls apart with the music.";
    }

    // LEVEL IDEA / CONCEPT
    if (
        containsInsensitive(text, "idea") ||
        containsInsensitive(text, "concept") ||
        containsInsensitive(text, "level idea")
    ) {
        return
            "Concept: the player travels through a world "
            "that is literally collapsing. Start with a "
            "quiet abandoned city, introduce cracks in "
            "reality, then turn the final drop into total "
            "environmental destruction.";
    }

    // NAMES
    if (
        containsInsensitive(text, "name") ||
        containsInsensitive(text, "level title")
    ) {
        return
            "Names you could use: Nullspace, Fracture, "
            "Afterglow, Eventide, Blackout, Last Light, "
            "Voidline, Redshift, Cataclysm, or Echofall.";
    }

    // GAMEPLAY
    if (
        containsInsensitive(text, "gameplay") ||
        containsInsensitive(text, "game play") ||
        containsInsensitive(text, "gp") ||
        containsInsensitive(text, "playstyle")
    ) {
        return
            "Give every section one main gameplay idea. "
            "Build around the song's strongest beats, "
            "then gradually increase timing precision "
            "and pattern complexity.";
    }

    // DECORATION
    if (
        containsInsensitive(text, "deco") ||
        containsInsensitive(text, "decoration")
    ) {
        return
            "Use three main depth layers: background, "
            "gameplay layer, and foreground. Repeat a "
            "small set of shapes and motifs so the level "
            "looks intentional instead of cluttered.";
    }

    // COLORS
    if (
        containsInsensitive(text, "color") ||
        containsInsensitive(text, "colour") ||
        containsInsensitive(text, "palette")
    ) {
        return
            "For a dark atmosphere, try charcoal and "
            "black as the base, pale white for contrast, "
            "and one bright accent color that becomes "
            "stronger during important parts of the song.";
    }

    // SONGS
    if (
        containsInsensitive(text, "song") ||
        containsInsensitive(text, "music") ||
        containsInsensitive(text, "nong")
    ) {
        return
            "Build the visuals around the song's energy. "
            "A soft intro followed by electric guitar or "
            "a huge drop works really well for a world-"
            "ending style level.";
    }

    // BOSS
    if (
        containsInsensitive(text, "boss") ||
        containsInsensitive(text, "monster")
    ) {
        return
            "Make the boss affect the actual level. "
            "Its attacks can move objects, change the "
            "background, trigger pulses, alter colors, "
            "or temporarily change the gameplay.";
    }

    // LORE
    if (
        containsInsensitive(text, "lore") ||
        containsInsensitive(text, "story")
    ) {
        return
            "Keep the story simple: discovery, escalation, "
            "collapse, then consequence. Let the "
            "environment reveal the story instead of "
            "using huge amounts of text.";
    }

    // TRANSITIONS
    if (
        containsInsensitive(text, "transition") ||
        containsInsensitive(text, "transitions")
    ) {
        return
            "For a strong transition, combine a short "
            "flash, camera movement, object pulses, "
            "color changes, and a visual element from "
            "the next section entering before the drop.";
    }

    // EFFECTS
    if (
        containsInsensitive(text, "effect") ||
        containsInsensitive(text, "effects")
    ) {
        return
            "Good effects for dramatic sections include "
            "screen pulses, subtle camera movement, "
            "background movement, particles, flashes, "
            "color grading, and objects reacting to beats.";
    }

    // DIFFICULTY
    if (
        containsInsensitive(text, "difficulty") ||
        containsInsensitive(text, "hard") ||
        containsInsensitive(text, "demon")
    ) {
        return
            "For a difficult level, make the gameplay "
            "challenging because of timing and patterns, "
            "not just because there are more objects. "
            "Give difficult sections readable setups.";
    }

    // HELP
    if (
        containsInsensitive(text, "help") ||
        containsInsensitive(text, "what can you do")
    ) {
        return
            "I can help with GD themes, concepts, names, "
            "gameplay, decoration, colors, songs, bosses, "
            "lore, transitions, effects, and difficulty.";
    }

    return
        "I'm your local Geometry Dash assistant. "
        "Ask me about themes, level ideas, names, "
        "gameplay, decoration, colors, songs, bosses, "
        "lore, transitions, effects, or difficulty.";
}


// ============================================================
// CHAT WINDOW
// ============================================================

class HelpifyChat : public CCLayer {

protected:

    CCLayerColor* m_background = nullptr;
    CCLabelBMFont* m_title = nullptr;
    CCLabelBMFont* m_reply = nullptr;

    TextInput* m_input = nullptr;

    CCMenu* m_menu = nullptr;
    CCMenuItemSpriteExtra* m_sendButton = nullptr;
    CCMenuItemSpriteExtra* m_closeButton = nullptr;

    float m_width = 420.f;
    float m_height = 290.f;

    bool m_dragging = false;
    bool m_resizing = false;

    CCPoint m_startTouch;
    CCPoint m_startPosition;

    float m_startWidth = 0.f;
    float m_startHeight = 0.f;


    bool init() override {

        if (!CCLayer::init())
            return false;

        this->setContentSize(
            CCSize(
                m_width,
                m_height
            )
        );

        buildUI();

        return true;
    }


    void registerWithTouchDispatcher() override {

        CCDirector::sharedDirector()
            ->getTouchDispatcher()
            ->addTargetedDelegate(
                this,
                -500,
                true
            );
    }


    void buildUI() {

        if (m_background) {

            m_background->removeFromParentAndCleanup(
                true
            );

            m_background = nullptr;
            m_title = nullptr;
            m_reply = nullptr;
            m_input = nullptr;
            m_menu = nullptr;
            m_sendButton = nullptr;
            m_closeButton = nullptr;
        }


        // BACKGROUND

        m_background =
            CCLayerColor::create(
                ccc4(
                    22,
                    22,
                    28,
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
                    42,
                    42,
                    50,
                    255
                ),
                m_width,
                42.f
            );

        header->setPosition(
            CCPoint(
                0.f,
                m_height - 42.f
            )
        );

        m_background->addChild(
            header,
            1
        );


        // TITLE

        m_title =
            CCLabelBMFont::create(
                assistantName().c_str(),
                "bigFont.fnt"
            );

        m_title->setScale(
            0.48f
        );

        m_title->setAnchorPoint(
            CCPoint(
                0.f,
                0.5f
            )
        );

        m_title->setPosition(
            CCPoint(
                12.f,
                21.f
            )
        );

        header->addChild(
            m_title
        );


        // MENU

        m_menu =
            CCMenu::create();

        m_menu->setPosition(
            CCPoint(
                0.f,
                0.f
            )
        );

        m_background->addChild(
            m_menu,
            10
        );


        // CLOSE BUTTON

        auto closeSprite =
            ButtonSprite::create(
                "X",
                "goldFont.fnt",
                "GJ_button_01.png",
                0.7f
            );

        m_closeButton =
            CCMenuItemSpriteExtra::create(
                closeSprite,
                this,
                menu_selector(
                    HelpifyChat::onClose
                )
            );

        m_closeButton->setPosition(
            CCPoint(
                m_width - 22.f,
                m_height - 21.f
            )
        );

        m_menu->addChild(
            m_closeButton
        );


        // RESPONSE

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
                m_width - 40.f
            )
        );

        m_reply->setLineBreakWithoutSpace(
            true
        );

        m_reply->setAnchorPoint(
            CCPoint(
                0.f,
                1.f
            )
        );

        m_reply->setPosition(
            CCPoint(
                20.f,
                m_height - 62.f
            )
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

        m_input =
            TextInput::create(
                inputWidth,
                "Ask Helpify...",
                "bigFont.fnt"
            );

        m_input->setMaxCharCount(
            180
        );

        m_input->setPosition(
            CCPoint(
                20.f + inputWidth / 2.f,
                28.f
            )
        );

        m_background->addChild(
            m_input,
            4
        );


        // SEND

        auto sendSprite =
            ButtonSprite::create(
                "SEND",
                "goldFont.fnt",
                "GJ_button_01.png",
                0.65f
            );

        m_sendButton =
            CCMenuItemSpriteExtra::create(
                sendSprite,
                this,
                menu_selector(
                    HelpifyChat::onSend
                )
            );

        m_sendButton->setPosition(
            CCPoint(
                m_width - 45.f,
                28.f
            )
        );

        m_menu->addChild(
            m_sendButton
        );


        // RESIZE HANDLE

        auto resizeHandle =
            CCLayerColor::create(
                ccc4(
                    255,
                    255,
                    255,
                    130
                ),
                16.f,
                16.f
            );

        resizeHandle->setPosition(
            CCPoint(
                m_width - 16.f,
                0.f
            )
        );

        resizeHandle->setID(
            "resize-handle"
        );

        m_background->addChild(
            resizeHandle,
            6
        );
    }


    bool isHeader(
        CCPoint const& p
    ) const {

        return
            p.x >= 0.f &&
            p.x <= m_width &&
            p.y >= m_height - 42.f &&
            p.y <= m_height;
    }


    bool isResizeCorner(
        CCPoint const& p
    ) const {

        return
            p.x >= m_width - 35.f &&
            p.x <= m_width + 5.f &&
            p.y >= -5.f &&
            p.y <= 35.f;
    }


    bool ccTouchBegan(
        CCTouch* touch,
        CCEvent*
    ) override {

        auto point =
            this->convertToNodeSpace(
                touch->getLocation()
            );


        // RESIZE

        if (isResizeCorner(point)) {

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


        // DRAG HEADER

        if (isHeader(point)) {

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

        auto current =
            touch->getLocation();


        // DRAG

        if (m_dragging) {

            float dx =
                current.x -
                m_startTouch.x;

            float dy =
                current.y -
                m_startTouch.y;


            auto win =
                CCDirector::sharedDirector()
                    ->getWinSize();


            float x =
                m_startPosition.x +
                dx;

            float y =
                m_startPosition.y +
                dy;


            float maxX =
                win.width -
                m_width;


            float maxY =
                win.height -
                m_height;


            x =
                std::max(
                    0.f,
                    std::min(
                        maxX,
                        x
                    )
                );


            y =
                std::max(
                    0.f,
                    std::min(
                        maxY,
                        y
                    )
                );


            this->setPosition(
                CCPoint(
                    x,
                    y
                )
            );
        }


        // RESIZE

        if (m_resizing) {

            float dx =
                current.x -
                m_startTouch.x;

            float dy =
                current.y -
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


            buildUI();
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


    void onSend(CCObject*) {

        if (!m_input)
            return;


        std::string message =
            m_input->getString();


        if (message.empty())
            return;


        if (containsSpecialName(message))
            playDing();


        auto answer =
            replyTo(message);


        std::string output =
            assistantName() +
            ": " +
            answer;


        if (m_reply) {

            m_reply->setString(
                output.c_str()
            );

            m_reply->setWidth(
                std::max(
                    100.f,
                    m_width - 40.f
                )
            );
        }


        m_input->setString(
            "",
            false
        );
    }


    void onClose(CCObject*) {

        this->removeFromParentAndCleanup(
            true
        );
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
// FLOATING HELPIFY ICON
// ============================================================

class HelpifyIconLayer : public CCLayer {

protected:

    CCSprite* m_icon = nullptr;

    bool m_dragging = false;

    CCPoint m_startTouch;
    CCPoint m_startPosition;


    bool init() override {

        if (!CCLayer::init())
            return false;


        auto win =
            CCDirector::sharedDirector()
                ->getWinSize();


        auto scale =
            static_cast<float>(
                Mod::get()->getSettingValue<double>(
                    "icon-scale"
                )
            );


        m_icon =
            CCSprite::create(
                "HelpifyIcon.png"_spr
            );


        if (!m_icon)
            return false;


        m_icon->setScale(
            0.16f * scale
        );


        this->addChild(
            m_icon
        );


        auto iconSize =
            m_icon->getContentSize();


        float finalScale =
            m_icon->getScale();


        this->setContentSize(
            CCSize(
                iconSize.width * finalScale,
                iconSize.height * finalScale
            )
        );


        // START TOP-RIGHT

        this->setPosition(
            CCPoint(
                win.width - 65.f,
                win.height - 65.f
            )
        );


        m_icon->setPosition(
            CCPoint(
                this->getContentSize().width / 2.f,
                this->getContentSize().height / 2.f
            )
        );


        return true;
    }


    void registerWithTouchDispatcher() override {

        CCDirector::sharedDirector()
            ->getTouchDispatcher()
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

        auto point =
            this->convertToNodeSpace(
                touch->getLocation()
            );


        auto size =
            this->getContentSize();


        if (
            point.x < 0.f ||
            point.y < 0.f ||
            point.x > size.width ||
            point.y > size.height
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

        auto current =
            touch->getLocation();


        float dx =
            current.x -
            m_startTouch.x;


        float dy =
            current.y -
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
            CCDirector::sharedDirector()
                ->getWinSize();


        float halfWidth =
            this->getContentSize().width /
            2.f;


        float halfHeight =
            this->getContentSize().height /
            2.f;


        float x =
            m_startPosition.x +
            dx;


        float y =
            m_startPosition.y +
            dy;


        x =
            std::max(
                halfWidth,
                std::min(
                    win.width - halfWidth,
                    x
                )
            );


        y =
            std::max(
                halfHeight,
                std::min(
                    win.height - halfHeight,
                    y
                )
            );


        this->setPosition(
            CCPoint(
                x,
                y
            )
        );
    }


    void ccTouchEnded(
        CCTouch*,
        CCEvent*
    ) override {

        // TAP

        if (!m_dragging) {

            auto chat =
                HelpifyChat::create();


            if (chat) {

                auto win =
                    CCDirector::sharedDirector()
                        ->getWinSize();


                chat->setPosition(
                    CCPoint(
                        std::max(
                            10.f,
                            win.width / 2.f -
                            210.f
                        ),
                        std::max(
                            10.f,
                            win.height / 2.f -
                            145.f
                        )
                    )
                );


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
// MENU LAYER
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
// LOADED
// ============================================================

$on_mod(Loaded) {

    log::info(
        "Helpify loaded successfully!"
    );
}

}
