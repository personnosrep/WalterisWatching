// first mod :)
#include <Geode/Geode.hpp>
#include <cocos2d.h>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>
#include <Geode/loader/SceneManager.hpp>

using namespace geode::prelude;
using namespace std::string_literals;

// Creates and returns a Walter sprite that fades in/out with timing options
cocos2d::CCSprite* waltuh(float fin = 0.5f, float fout = 0.25f, float waitt = 0.0f, float targetOpacity = 255.0f) {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto overlay = cocos2d::CCSprite::create("waltuh.png"_spr);
    if (!overlay) return nullptr;

    overlay->setPosition({ winSize.width / 2, winSize.height / 2 });

    // Scale overlay to cover screen fully, keep aspect ratio
    float scaleX = winSize.width / overlay->getContentSize().width;
    float scaleY = winSize.height / overlay->getContentSize().height;
    overlay->setScale(winSize.width > winSize.height ? scaleX : scaleY);

    overlay->setOpacity(fin > 0 ? 0 : static_cast<GLubyte>(targetOpacity));

    cocos2d::CCDirector::sharedDirector()->getRunningScene()->addChild(overlay, 1);

    // Run fade in, wait, fade out sequence
    auto fadeIn = cocos2d::CCFadeTo::create(fin, targetOpacity);
    auto wait = cocos2d::CCDelayTime::create(waitt);
    auto fadeOut = cocos2d::CCFadeTo::create(fout, 0);
    auto sequence = cocos2d::CCSequence::create(fadeIn, wait, fadeOut, nullptr);
    overlay->runAction(sequence);

    return overlay;
}

// Play sound 
void playsound(std::string path = "", float volume = FMODAudioEngine::get()->getEffectsVolume() * 2) {
    auto system = FMODAudioEngine::get()->m_system;
    FMOD::Channel* channel;
    FMOD::Sound* sound;
    system->createSound((Mod::get()->getResourcesDir() / path).string().c_str(), FMOD_DEFAULT, nullptr, &sound);
    system->playSound(sound, nullptr, false, &channel);
    channel->setVolume(volume);
}

// fixed; now persistent WalterGhost node
class WalterGhost : public cocos2d::CCNode {
public:
    bool init() override {
        if (!CCNode::init())
            return false;

        auto waltSprite = cocos2d::CCSprite::create("waltuh.png"_spr);
        auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
        waltSprite->setPosition(winSize / 2);

        float scaleX = winSize.width / waltSprite->getContentSize().width;
        float scaleY = winSize.height / waltSprite->getContentSize().height;
        waltSprite->setScale(winSize.width > winSize.height ? scaleX : scaleY);

        waltSprite->setOpacity(4);  // Very faint ghost

        this->addChild(waltSprite);
        return true;
    }

    CREATE_FUNC(WalterGhost);
};

// Add the WalterGhost node once mod loads
class WalterIsWatchingMod : public Mod {
public:
    void onLoad() override {
        geode::SceneManager::get()->addPersistentNode(WalterGhost::create());
    }
};

static WalterIsWatchingMod mod;

// When completing a level
class $modify(PlayLayer) {
    void levelComplete() {
        PlayLayer::levelComplete();
        playsound("i won.mp3");
        waltuh(0.5f, 1.0f, 0.5f);
    }
};

// Walter reacts on death
class $modify(PlayerObject) {
    void playDeathEffect() {
        PlayerObject::playDeathEffect();
        waltuh(0, 0.25f);
        playsound("waow.mp3", FMODAudioEngine::get()->getEffectsVolume() * 0.075);
    }
};

// Replace Facebook button with Walter sprite in the main menu
class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        auto sc = this->getChildByID("social-media-menu");
        if (!sc) {
            log::warn("social-media-menu not found in MenuLayer");
            return true;
        }

        auto facebook = sc->getChildByID("facebook-button");
        if (facebook) {
            auto pos = facebook->getPosition();
            auto sz = facebook->getScale();

			// remove button and copy with walter 
			// nobody uses the facebook button please let this one slide

            facebook->removeFromParentAndCleanup(true);
            auto newSprite = cocos2d::CCSprite::create("extreme-walter.png"_spr);

            newSprite->setPosition(pos);
            newSprite->setScale(sz * 0.125f);
            newSprite->setID("facebook-button");

            sc->addChild(newSprite);
        } else {
            log::warn("facebook-button not found in MenuLayer");
        }

        return true;
    }
};

// Replace difficulty sprite with Walter for a specific level
class $modify(LevelInfoLayer) {
    bool init(GJGameLevel* level, bool p1) {
        if (!LevelInfoLayer::init(level, p1))
            return false;

        auto levelname = level->m_levelID.value();

        if (levelname == 81011195) {
            auto face = this->getChildByID("difficulty-sprite");

            if (face) {
                auto pos = face->getPosition();
                pos.x += 11;
                auto sz = face->getScale();

                face->removeFromParentAndCleanup(true);
                auto newSprite = cocos2d::CCSprite::create("extreme-walter.png"_spr);

                newSprite->setPosition(pos);
                newSprite->setScale(sz * 0.25f);
                newSprite->setID("difficulty-sprite");

                this->addChild(newSprite);
            } else {
                log::warn("difficulty-sprite not found in LevelInfoLayer");
            }
        }

        return true;
    }
};
