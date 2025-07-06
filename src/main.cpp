// first mod :)
#include <Geode/Geode.hpp>
#include <cocos2d.h>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/CCMenuItem.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/SceneManager.hpp>

using namespace geode::prelude;
using namespace std::string_literals;

// Walter appears
cocos2d::CCSprite* waltuh(float fin = 0.5f, float fout = 0.25f, float waitt = 0.0f, float targetOpacity = 255.0f) {
	// get the window size and sprite
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto overlay = cocos2d::CCSprite::create("waltuh.png"_spr);

    if (overlay) {
        overlay->setPosition({ winSize.width / 2, winSize.height / 2 });

		// Scale the overlay to fit the screen
        float scaleX = winSize.width / overlay->getContentSize().width;
        float scaleY = winSize.height / overlay->getContentSize().height;
        overlay->setScale(winSize.width > winSize.height ? scaleX : scaleY);

        overlay->setOpacity(fin > 0 ? 0 : static_cast<GLubyte>(targetOpacity)); 
        cocos2d::CCDirector::sharedDirector()->getRunningScene()->addChild(overlay, 1);

		// Create fade in, wait, and fade out action
        auto fadeIn = cocos2d::CCFadeTo::create(fin, targetOpacity);
        auto wait = cocos2d::CCDelayTime::create(waitt); 
        auto fadeOut = cocos2d::CCFadeTo::create(fout, 0); 
        auto sequence = cocos2d::CCSequence::create(fadeIn, wait, fadeOut, nullptr);
        overlay->runAction(sequence);
    }

    return overlay;
}

// sounds
void playsound(std::string path = "", float volume = FMODAudioEngine::get()->getEffectsVolume() * 2) {
	auto system = FMODAudioEngine::get()->m_system;
	FMOD::Channel* channel;
	FMOD::Sound* sound;
	system->createSound((Mod::get()->getResourcesDir() / path).string().c_str(), FMOD_DEFAULT, nullptr, &sound);
	system->playSound(sound, nullptr, false, &channel);
	channel->setVolume(volume);
}

class $modify(CCDirector) {
    void drawScene() {
        CCDirector::drawScene();

        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (scene && !scene->getChildByTag(42069)) {
            // Create ghost only once per game run
            static cocos2d::CCSprite* ghost = nullptr;

            if (!ghost) {
                ghost = waltuh(0, 0, 9999999 * 999, 4);
                if (ghost) {
                    ghost->setTag(42069);
                    // persistent across scenes
                    geode::SceneManager::get()->keepAcrossScenes(ghost);
                }
            }
            // If ghost exists but not added to current scene
            if (ghost && ghost->getParent() != scene) {
                ghost->retain();
                ghost->removeFromParent();
                scene->addChild(ghost);
                ghost->release();
            }
        }
    }
};


// Walter appears on menu button press
// Mainly just transitions
class $modify(CCMenuItem) {
    void activate() {
        CCMenuItem::activate();
        waltuh(0.2f, 0.2f); 
    };
};

// When completing a level
class $modify(PlayLayer) {
    void levelComplete() {
        PlayLayer::levelComplete();
        playsound("i won.mp3");
        waltuh(0.5f, 1.0f, 0.5f); 
    }
};

// after dying
class $modify(PlayerObject) {
    void playDeathEffect() {
        PlayerObject::playDeathEffect();
        waltuh(0, 0.25f); 
        playsound("waow.mp3", FMODAudioEngine::get()->getEffectsVolume() * 0.075);
    }
};

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

		// Create walter for the facebook button

        auto sc = this->getChildByID("social-media-menu");
		auto facebook = sc->getChildByID("facebook-button");
        if (facebook) {

			// remove button and copy with walter 
			// nobody uses the facebook button please let this one slide

            auto pos = facebook->getPosition();
            auto sz = facebook->getScale();

            facebook->removeFromParentAndCleanup(true);
            auto newSprite = cocos2d::CCSprite::create("extreme-walter.png"_spr);

			// copy position and scale from the original facebook button
            newSprite->setPosition(pos);
            newSprite->setScale(sz * 0.125f);

			newSprite->setID("facebook-button");

            sc->addChild(newSprite);
        } else {
            log::warn("failed to find facebook-button in MenuLayer");
        }

        return true;
    }
};

class $modify(LevelInfoLayer) {
    bool init(GJGameLevel* level, bool p1) {

        if (!LevelInfoLayer::init(level, p1)) {
            return false;
        }

		auto levelname = level->m_levelID.value();

		// Walter White extreme 
		if (levelname == 81011195) {
			auto face = this->getChildByID("difficulty-sprite");

			if (face) {

				// remove the original difficulty sprite and replace it with walter
				auto pos = face->getPosition();
				pos.x += 11;
				auto sz = face->getScale();

				face->removeFromParentAndCleanup(true);
				auto newSprite = cocos2d::CCSprite::create("extreme-walter.png"_spr);
				
				// Set the position and scale of the new sprite
				newSprite->setPosition(pos);
				newSprite->setScale(sz * 0.25f);

				newSprite->setID("difficulty-sprite");

				this->addChild(newSprite);

			} else {
				log::warn("Unable to find difficulty-sprite in LevelInfoLayer");
			}

        return true;
    	};
	return true;
	};
};
