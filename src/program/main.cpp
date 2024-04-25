
#include "rs/util.hpp"

#include "lib.hpp"
#include "al/MathRandomUtil.h"
#include "al/util/LiveActorUtil.h"
#include "al/factory/ActorFactory.h"
#include "game/StageScene/StageScene.h"

/*
 The development name of the actor to spawn (KuriboHack for goomba and Togezo for spiny)
 For the dev names pf all actors see: https://docs.google.com/spreadsheets/d/1PbmqLtUQLPiPaHKS5DD6K4-UQNX3JfSgYI1OKeXAwwk
 */

#define ENEMY_TYPE KuriboHack

/*
 The maximum amount of enemies at once.
 If the limit is passed when spawning an enemy it will despwan the last enemy.
 If the game is crashing or lagging, then decrease this.
 */

const int MAX_ENEMIES = 50;

const int ENEMY_DISTANCE = 400; //Distance of each enemy to mario (maybe pixels?)

const int ENEMY_RATE = 15; // (15 frames = 4 every seccond) change this to change how frequently enemies spawn (in frames).

 sead::Vector3f RandomVector3f() {
    float x = (al::getRandom() -0.5) * ENEMY_DISTANCE;
    float y = 300.0f;
    float auxrandom = al::getRandom() - 0.5;
    float z;
    if (auxrandom > 0) {
    z = ENEMY_DISTANCE - (x*x)/(2*ENEMY_DISTANCE+1);
    } else {
        z = (-1) * (ENEMY_DISTANCE - (x*x)/(2*ENEMY_DISTANCE+1));
    }
    return sead::Vector3f{x, y, z};
}


al::LiveActor* enemies[MAX_ENEMIES];

class ENEMY_TYPE;

HOOK_DEFINE_TRAMPOLINE(SceneInit){
    static void Callback(al::ActorInitInfo* info, StageScene* curScene, al::PlacementInfo const* placement, al::LayoutInitInfo const* lytInfo,
                                                 al::ActorFactory const* factory, al::SceneMsgCtrl* sceneMsgCtrl, al::GameDataHolderBase* dataHolder) {

        al::initActorInitInfo(info, curScene, placement, lytInfo, factory, sceneMsgCtrl, dataHolder);
        for(int i = 0; i < MAX_ENEMIES; i++){
            enemies[i] = al::createActorFunction<ENEMY_TYPE>("spiny");

            al::initCreateActorNoPlacementInfo(enemies[i], *info);
            setTrans(enemies[i], {0.f, -50000.f, 0.f});
            enemies[i]->makeActorDead();
        }
        Orig(info, curScene, placement, lytInfo, factory, sceneMsgCtrl, dataHolder);
    }
};

int frameCounter = 0;
int currentEnemyIndex = 0;

HOOK_DEFINE_TRAMPOLINE(UpdateHook) {
    static void Callback(StageScene *scene) {
        if(scene->mIsAlive && !scene->isPause()) {

            auto player = rs::getPlayerActor(scene);
            if (frameCounter % ENEMY_RATE == 0) {
                auto playerTrans = al::getTrans(player);
                auto enemyTrans = playerTrans + RandomVector3f();
                enemies[currentEnemyIndex]->makeActorAlive();
                al::setTrans(enemies[currentEnemyIndex], enemyTrans);
                enemies[currentEnemyIndex]->appear();
                currentEnemyIndex++;
                if (currentEnemyIndex >= MAX_ENEMIES) currentEnemyIndex = 0;
            }
            frameCounter++;
        }
        Orig(scene);
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    // Setup hooking enviroment.
    exl::hook::Initialize();

    SceneInit::InstallAtOffset(0x4C8DD0);
    //R_ABORT_UNLESS(Logger::instance().init(LOGGER_IP, 3080, false));
    //InitSpinies::InstallAtOffset(0x41B384);
    UpdateHook::InstallAtSymbol("_ZN10StageScene7controlEv");
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}


