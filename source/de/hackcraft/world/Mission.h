/**
 * File:     Mission.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 10:23 PM
 */

#ifndef _CMISSION_H
#define _CMISSION_H

class Mission;

#include "World.h"

#include "de/hackcraft/world/Entity.h"
#include "de/hackcraft/world/object/cPlanetmap.h"

#include "de/hackcraft/io/Pad.h"

#include <vector>
#include <string>

/**
 * Missions are responsible for initial and midtime spawning of objects
 * (including the player object), tracking mission objectives (frags).
 *
 * Future: support for timetriggers like onTime().
 *
 */
class Mission {
protected:
    // Mission Objectives
    std::vector<Entity*> mVictory; // Shall be destroyed
    std::vector<Entity*> mDefeat; // May not be destroyed includes player
    int mState;
public:

    Mission() : mState(0) {
    };

    /**
     * Called with the world which needs to be populated by this mission.
     * So this method should spawn objects into the world and setup
     * other mission critical data of the derived mission.
     */
    virtual Entity* init(World* world);

    /**
     * Through this method mission objectives can be traced.
     * Victory (ANDed) and Defeat (ORed) Conditions are checked.
     * onVictory or onDefeat are called accordingly.
     * Enhance as desired.
     */
    virtual void checkConditions();

    /// Called when checkConditions() sees the victory condition(s) fullfilled.
    virtual void onVictory();

    /// Called when checkConditions() sees the defeat condition(s) fullfilled.
    virtual void onDefeat();

    /**
     * Indicates wether this Mission is a success.
     */
    //virtual bool isSuccess() = 0;

    /**
     * Indicates wether this Mission is a failure.
     */
    //virtual bool isFailure() = 0;
};

#endif

