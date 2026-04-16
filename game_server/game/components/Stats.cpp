/// @author    Chnossos
/// @date      Created on 2026-04-16

#include "Stats.hpp"

void Stats::compute(Actor const &)
{
    STR = baseSTR + bonusSTR;
    DEX = baseDEX + bonusDEX;
    CON = baseCON + bonusCON;
    INT = baseINT + bonusINT;
    WIT = baseWIT + bonusWIT;
    MEN = baseMEN + bonusMEN;

    pAtkSpeedMultiplier = 1. + (DEX - 20) / 100.;
    moveSpeedMultiplier = 1. + (DEX - 20) / 100.;

    pAtkSpeed = static_cast<StatValue>(pAtkSpeedBase * pAtkSpeedMultiplier + pAtkSpeedBonus);
    runSpeed  = static_cast<StatValue>(runSpeedBase  * moveSpeedMultiplier + moveSpeedBonus);

    maxHp             = static_cast<StatValue>(maxHpBase             * maxHpMultiplier             + maxHpBonus);
    hpRegen           = static_cast<StatValue>(hpRegenBase           * hpRegenMultiplier           + hpRegenBonus);
    maxMp             = static_cast<StatValue>(maxMpBase             * maxMpMultiplier             + maxMpBonus);
    mpRegen           = static_cast<StatValue>(mpRegenBase           * mpRegenMultiplier           + mpRegenBonus);
    maxCp             = static_cast<StatValue>(maxCpBase             * maxCpMultiplier             + maxCpBonus);
    cpRegen           = static_cast<StatValue>(cpRegenBase           * cpRegenMultiplier           + cpRegenBonus);
    pAtk              = static_cast<StatValue>(pAtkBase              * pAtkMultiplier              + pAtkBonus);
    pDef              = static_cast<StatValue>(pDefBase              * pDefMultiplier              + pDefBonus);
    mAtk              = static_cast<StatValue>(mAtkBase              * mAtkMultiplier              + mAtkBonus);
    mDef              = static_cast<StatValue>(mDefBase              * mDefMultiplier              + mDefBonus);
    pAtkSpeed         = static_cast<StatValue>(pAtkSpeedBase         * pAtkSpeedMultiplier         + pAtkSpeedBonus);
    mAtkSpeed         = static_cast<StatValue>(mAtkSpeedBase         * mAtkSpeedMultiplier         + mAtkSpeedBonus);
    pAtkRange         = static_cast<StatValue>(pAtkRangeBase         * pAtkRangeMultiplier         + pAtkRangeBonus);
    pAtkRandom        = static_cast<StatValue>(pAtkRandomBase        * pAtkRandomMultiplier        + pAtkRandomBonus);
    accuracy          = static_cast<StatValue>(accuracyBase          * accuracyMultiplier          + accuracyBonus);
    evasion           = static_cast<StatValue>(evasionBase           * evasionMultiplier           + evasionBonus);
    pCritRate         = static_cast<StatValue>(pCritRateBase         * pCritRateMultiplier         + pCritRateBonus);
    mCritRate         = static_cast<StatValue>(mCritRateBase         * mCritRateMultiplier         + mCritRateBonus);
    moveSpeed         = static_cast<StatValue>(moveSpeedBase         * moveSpeedMultiplier         + moveSpeedBonus);
    maxWeight         = static_cast<StatValue>(maxWeightBase         * maxWeightMultiplier         + maxWeightBonus);
    inventoryLimit    = static_cast<StatValue>(inventoryLimitBase    * inventoryLimitMultiplier    + inventoryLimitBonus);
    wharehouseLimit   = static_cast<StatValue>(wharehouseLimitBase   * wharehouseLimitMultiplier   + wharehouseLimitBonus);
    freightLimit      = static_cast<StatValue>(freightLimitBase      * freightLimitMultiplier      + freightLimitBonus);
    storeLimit        = static_cast<StatValue>(storeLimitBase        * storeLimitMultiplier        + storeLimitBonus);
    commonCraftLimit  = static_cast<StatValue>(commonCraftLimitBase  * commonCraftLimitMultiplier  + commonCraftLimitBonus);
    dwarvenCraftLimit = static_cast<StatValue>(dwarvenCraftLimitBase * dwarvenCraftLimitMultiplier + dwarvenCraftLimitBonus);
}
