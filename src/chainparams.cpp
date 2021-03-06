// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <cassert>

#include "chainparamsseeds.h"

// Far into the future.
static const std::string ANTI_REPLAY_COMMITMENT =
    "Bitcoin Core: Satoshi's True Vision";

static std::vector<uint8_t> GetAntiReplayCommitment() {
    return std::vector<uint8_t>(std::begin(ANTI_REPLAY_COMMITMENT),
                                std::end(ANTI_REPLAY_COMMITMENT));
}

static CBlock CreateGenesisBlock(const char *pszTimestamp,
                                 const CScript &genesisOutputScript,
                                 uint32_t nTime, uint32_t nNonce,
                                 uint32_t nBits, int32_t nVersion,
                                 const Amount genesisReward) {
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig =
        CScript() << 486604799 << CScriptNum(4)
                  << std::vector<uint8_t>((const uint8_t *)pszTimestamp,
                                          (const uint8_t *)pszTimestamp +
                                              strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime = nTime;
    genesis.nBits = nBits;
    genesis.nNonce = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation transaction
 * cannot be spent since it did not originally exist in the database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000,
 * hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893,
 * vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase
 * 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce,
                                 uint32_t nBits, int32_t nVersion,
                                 const Amount genesisReward) {
    const char *pszTimestamp =
        "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
    const CScript genesisOutputScript =
        CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909"
                              "a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112"
                              "de5c384df7ba0b8d578a4c702b6bf11d5f")
                  << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce,
                              nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.nSubsidyHalvingIntervalOneMinute = 210000 * 10;
        consensus.BIP34Height = 227931;
        consensus.BIP34Hash = uint256S("0x000000000000024b89b42a942fe0d9fea3bb4"
                                       "4ab7bd1b19115dd6a759c0808b8");
        // 000000000000000004c2b624ed5d7756c508d90fd0da2c7c679febfa6c4735f0
        consensus.BIP65Height = 388381;
        // 00000000000000000379eaa19dce8c9b722d46ae6a57c2f1a988119488b50931
        consensus.BIP66Height = 363725;
        consensus.antiReplayOpReturnSunsetHeight = 530000;
        consensus.antiReplayOpReturnCommitment = GetAntiReplayCommitment();
        consensus.powLimit = uint256S(
            "00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        // two weeks
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60;
        consensus.nPowTargetSpacing = 10 * 60;
        // hardfork to one min blocks
        consensus.nPowTargetSpacingOneMinute = 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        // 95% of 2016
        consensus.nRuleChangeActivationThreshold = 1916;
        // nPowTargetTimespan / nPowTargetSpacing
        consensus.nMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime =
            1199145601;
        // December 31, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout =
            1230767999;

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        // May 1st, 2016
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime =
            1462060800;
        // May 1st, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1493596800;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork =
            uint256S("0x0000000000000000000000000000000000000000007cb5f207557941bd9a6096");

        // By default assume that the signatures in ancestors of this block are
        // valid. Block Height #631,515
        consensus.defaultAssumeValid =
            uint256S("0x0000000000000020f1b468fb83467c927d3db9a436eec0bc2d6b6b27fb3c0904");

        // Aug, 1 hard fork
        consensus.uahfHeight = 478559;

        // May, 21st hard fork. Human time (GMT): Monday, May 21, 2018 04:00:00 PM
        // Actual fork was 5 hours later
        consensus.coreHardForkActivationTime = 1526852960;

        // Reject PROTOCOL_VERSION 80030 Time
        // Jun 06, 2019 14:00:00 GMT Enforce PROTOCOL_VERSION=80050
        consensus.enforceProtocolVersion80050Time = 1559815200;

        // At this height we will hardfork to 1-minute blocks and 30-period DAA
        consensus.oneMinuteBlockHeight = 588672;

        // At this height we will hardfork to Blake2b PoW algo
        consensus.powBlake2Height = 735000;

        // Take the amount of 10-minute blocks in this interval and add it
        // to the number of expected 1-minute blocks left in the orginal planned
        // interval to figure out when to cut the block subsidy. It should be more 
        // straight forward on subsequent halvings.
        // ie. (((588672 -(210000*2))+(((210000*3)-588672)*10)))=168672+(41328*10)
        // ie. (((586656 -(210000*2))+(((210000*3)-586656)*10)))=166656+(43344*10)
        // so this interval would be 581952 blocks past last halving
        // which means 12.5 / 10 / 2 block rewards at a height of 210000 * 2 + 581952
        // or halve to 0.625 TNET at height 1,001,952
        consensus.nSubsidyHalvingIntervalOneMinuteAdjustment =
            (((consensus.oneMinuteBlockHeight - (210000 * 2)) + 
            (((210000 * 3) - consensus.oneMinuteBlockHeight) * 10)));

        /**
         * The message start string is designed to be unlikely to occur in
         * normal data. The characters are rarely used upper ASCII, not valid as
         * UTF-8, and produce a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xf9;
        pchMessageStart[1] = 0xbe;
        pchMessageStart[2] = 0xb4;
        pchMessageStart[3] = 0xd9;
        pchCashMessageStart[0] = 0xe3;
        pchCashMessageStart[1] = 0xe1;
        pchCashMessageStart[2] = 0xf3;
        pchCashMessageStart[3] = 0xe8;
        nDefaultPort = 10333;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1231006505, 2083236893, 0x1d00ffff, 1,
                                     50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock ==
               uint256S("0x000000000019d6689c085ae165831e934ff763ae46a2a6c172b3"
                        "f1b60a8ce26f"));
        assert(genesis.hashMerkleRoot ==
               uint256S("0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab212"
                        "7b7afdeda33b"));

        // Note that of those with the service bits flag, most only support a
        // subset of possible options.
        // clashic.cash maintained by Clashicly
        vSeeds.push_back(
            CDNSSeedData("clashic.cash", "seeder.clashic.cash", true));
        // bitcoincore.zone
        vSeeds.push_back(
            CDNSSeedData("bitcoincore.zone", "seeder.bitcoincore.zone", true));
        // truevisionofsatoshi.com
        vSeeds.push_back(
            CDNSSeedData("clashic.org", "seeder-mainnet.clashic.org", true));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<uint8_t>(1, 0);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<uint8_t>(1, 5);
        base58Prefixes[SECRET_KEY] = std::vector<uint8_t>(1, 128);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};
        titleaddrPrefix = "tnet";

        vFixedSeeds = std::vector<SeedSpec6>(
            pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;

        checkpointData = {
            .mapCheckpoints = {
                {11111, uint256S("0x0000000069e244f73d78e8fd29ba2fd2ed618bd6fa2"
                                 "ee92559f542fdb26e7c1d")},
                {33333, uint256S("0x000000002dd5588a74784eaa7ab0507a18ad16a236e"
                                 "7b1ce69f00d7ddfb5d0a6")},
                {74000, uint256S("0x0000000000573993a3c9e41ce34471c079dcf5f52a0"
                                 "e824a81e7f953b8661a20")},
                {105000, uint256S("0x00000000000291ce28027faea320c8d2b054b2e0fe"
                                  "44a773f3eefb151d6bdc97")},
                {134444, uint256S("0x00000000000005b12ffd4cd315cd34ffd4a594f430"
                                  "ac814c91184a0d42d2b0fe")},
                {168000, uint256S("0x000000000000099e61ea72015e79632f216fe6cb33"
                                  "d7899acb35b75c8303b763")},
                {193000, uint256S("0x000000000000059f452a5f7340de6682a977387c17"
                                  "010ff6e6c3bd83ca8b1317")},
                {210000, uint256S("0x000000000000048b95347e83192f69cf0366076336"
                                  "c639f9b7228e9ba171342e")},
                {216116, uint256S("0x00000000000001b4f4b433e81ee46494af945cf960"
                                  "14816a4e2370f11b23df4e")},
                {225430, uint256S("0x00000000000001c108384350f74090433e7fcf79a6"
                                  "06b8e797f065b130575932")},
                {250000, uint256S("0x000000000000003887df1f29024b06fc2200b55f8a"
                                  "f8f35453d7be294df2d214")},
                {279000, uint256S("0x0000000000000001ae8c72a0b0c301f67e3afca10e"
                                  "819efa9041e458e9bd7e40")},
                {295000, uint256S("0x00000000000000004d9b4ef50f0f9d686fd69db2e0"
                                  "3af35a100370c64632a983")},
                // UAHF fork block
                {478559, uint256S("0x000000000000000000651ef99cb9fcbe0dadde1d42"
                                  "4bd9f15ff20136191a5eec")},
                // Sept 30, 2017
                {490000, uint256S("0x0000000000000000018ade0e75b4c21db72f05db1e"
                                  "4fffb870c26d6c765dc6d1")},
                // May 18, 2018
                {575000, uint256S("0x000000000000000e507218030ced7824e0a6b84e6d"
                                  "243bc76d6590930fc88c82")},
                // May 23, 2018
                {576720, uint256S("0x00000000000000017de31d8e9a74ba7c50557d13e1"
                                  "0290131dfa78202799faea")},
                // July 26, 2018
                {585550, uint256S("0x0000000000000004fb959c2bbabb38609bbe44fa4b"
                                  "b206b1a81849f9b10414cd")},
                // Sept 5, 2018
                {615385, uint256S("0x000000000000000f61a5f419643ce6c15a579f4928"
                                  "9d6fd1b1ed521f899d44ee")},
                // Jan 17, 2019
                {734750, uint256S("0x000000000000000eea84db6a33adefff7cdb4ee87e"
                                  "c961a8640b0de8b0df363d")},
                // May 12, 2019
                {819300, uint256S("0x00000000000000921ef5597db254728192397d6a40"
                                  "eaf5b502da4a4538bedb5a")}}};


        // Data as of block
        // 000000000000000f61a5f419643ce6c15a579f49289d6fd1b1ed521f899d44ee
        // (height 615385).
        chainTxData = ChainTxData{
            // UNIX timestamp of last known number of transactions.
            1536198411,
            // Total number of transactions between genesis and that timestamp
            // (the tx=... number in the SetBestChain debug.log lines)
            245734254,
            // Estimated number of transactions per second after that timestamp.
            0.01};
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.nSubsidyHalvingIntervalOneMinute = 210000 * 10;
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256S("0x0273ed223240099a339d351d46054156fb6f3"
                                       "855c5db1c00c0275c9b6f1d07de");
        // 00000000007f6655f22f98e72ed80d8b06dc761d5da09df0fa1dc4be4f861eb6
        consensus.BIP65Height = 0;
        // 000000002104c8c45e99a8853285a3b592602a3ccde2b832481da85e9e4ba182
        consensus.BIP66Height = 0;
        consensus.antiReplayOpReturnSunsetHeight = 1;
        consensus.antiReplayOpReturnCommitment = GetAntiReplayCommitment();
        consensus.powLimit = uint256S(
            "1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        // two weeks
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60;
        consensus.nPowTargetSpacing = 10 * 60;
        // hardfork to one min blocks
        consensus.nPowTargetSpacingOneMinute = 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        // 75% for testchains
        consensus.nRuleChangeActivationThreshold = 1512;
        // nPowTargetTimespan / nPowTargetSpacing
        consensus.nMinerConfirmationWindow = 2016;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime =
            1199145601;
        // December 31, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout =
            1230767999;

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        // March 1st, 2016
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime =
            1456790400;
        // May 1st, 2017
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1493596800;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork =
            uint256S("0x0000000000000000000000000000000000000000000000000000000"
                     "00000032f");

        // By default assume that the signatures in ancestors of this block are
        // valid.
        consensus.defaultAssumeValid =
            uint256S("0x02b4614f9a5ddb8937835e4b871fccda4bcdd9741f349005444e8c8"
                     "4a8cfbcc8");

        // Aug, 1 hard fork
        consensus.uahfHeight = 1;

        // May, 21st hard fork
        consensus.coreHardForkActivationTime = 1526860800;

        // Reject PROTOCOL_VERSION 80030 Time
        consensus.enforceProtocolVersion80050Time = 1532581620;
 
        // At this height we will hardfork to 1-minute blocks and 30-period DAA
        consensus.oneMinuteBlockHeight = 160;

        // At this height we will hardfork to Blake2b PoW algo
        consensus.powBlake2Height = 250000;

        pchMessageStart[0] = 0x0b;
        pchMessageStart[1] = 0x11;
        pchMessageStart[2] = 0x09;
        pchMessageStart[3] = 0x07;
        pchCashMessageStart[0] = 0xf4;
        pchCashMessageStart[1] = 0xe5;
        pchCashMessageStart[2] = 0xf3;
        pchCashMessageStart[3] = 0xf4;
        nDefaultPort = 20333;
        nPruneAfterHeight = 1000;

        genesis =
            CreateGenesisBlock(1531239140, 28, 0x20111111, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock ==
               uint256S("0x0273ed223240099a339d351d46054156fb6f3855c5db1c00c027"
                        "5c9b6f1d07de"));
        assert(genesis.hashMerkleRoot ==
               uint256S("0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab212"
                        "7b7afdeda33b"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        // truevisionofsatoshi.com
        vSeeds.push_back(CDNSSeedData("truevisionofsatoshi.com",
                                      "seeder-testnet.truevisionofsatoshi.com", true));
        // clashic.cash maintained by Clashicly
        vSeeds.push_back(
            CDNSSeedData("clashic.cash", "testnet-seeder.clashic.cash", true));
        // bitcoincore.zone
        vSeeds.push_back(
            CDNSSeedData("bitcoincore.zone", "testnet-seeder.bitcoincore.zone", true));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<uint8_t>(1, 111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<uint8_t>(1, 196);
        base58Prefixes[SECRET_KEY] = std::vector<uint8_t>(1, 239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};
        titleaddrPrefix = "tnettest";
        vFixedSeeds = std::vector<SeedSpec6>(
            pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;

        checkpointData = {
            .mapCheckpoints = {
                // Bitcoin Core Testnet Genesis block
                {0, uint256S("0x0273ed223240099a339d351d46054156fb6f3855c5db1c00c027"
                             "5c9b6f1d07de")},
                // Just past 2nd new testnet halving
                {100, uint256S("0x16420994c95bcf9afa2fab4a45af3cfb5dc6ac6103a40e2"
                               "ffa48e7d327bdd7f")},
            }};

        // Data as of block
        // 02b4614f9a5ddb8937835e4b871fccda4bcdd9741f349005444e8c84a8cfbcc8
        // (height 421382)
        chainTxData = ChainTxData{1531625001, 421382, 1.09};
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        // BIP34 has not activated on regtest (far in the future so block v1 are
        // not rejected in tests)
        consensus.BIP34Height = 100000000;
        consensus.BIP34Hash = uint256();
        // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP65Height = 1351;
        // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251;
        consensus.antiReplayOpReturnSunsetHeight = 530000;
        consensus.antiReplayOpReturnCommitment = GetAntiReplayCommitment();
        consensus.powLimit = uint256S(
            "7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        // two weeks
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60;
        consensus.nPowTargetSpacing = 10 * 60;
        // hardfork to one min blocks
        consensus.nPowTargetSpacingOneMinute = 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        // 75% for testchains
        consensus.nRuleChangeActivationThreshold = 108;
        // Faster than normal for regtest (144 instead of 2016)
        consensus.nMinerConfirmationWindow = 144;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout =
            999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout =
            999999999999ULL;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are
        // valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        // Hard fork is always enabled on regtest.
        consensus.uahfHeight = 0;

        // Nov, 13 hard fork
        consensus.coreHardForkActivationTime = 0;

        // 1-minute blocks are always enabled on regtest
        consensus.oneMinuteBlockHeight = 0;

        // Blake2b PoW is always enabled on regtest
        consensus.powBlake2Height = 0;

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        pchCashMessageStart[0] = 0xda;
        pchCashMessageStart[1] = 0xb5;
        pchCashMessageStart[2] = 0xbf;
        pchCashMessageStart[3] = 0xfa;
        nDefaultPort = 20444;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1296688602, 2, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock ==
               uint256S("0x0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b"
                        "1a11466e2206"));
        assert(genesis.hashMerkleRoot ==
               uint256S("0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab212"
                        "7b7afdeda33b"));

        //!< Regtest mode doesn't have any fixed seeds.
        vFixedSeeds.clear();
        //!< Regtest mode doesn't have any DNS seeds.
        vSeeds.clear();

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;

        checkpointData = {.mapCheckpoints = {
                              {0, uint256S("0f9188f13cb7b2c71f2a335e3a4fc328bf5"
                                           "beb436012afca590b1a11466e2206")},
                          }};

        chainTxData = ChainTxData{0, 0, 0};

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<uint8_t>(1, 111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<uint8_t>(1, 196);
        base58Prefixes[SECRET_KEY] = std::vector<uint8_t>(1, 239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};
        titleaddrPrefix = "tnetreg";
    }

    void UpdateBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime,
                              int64_t nTimeout) {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
};

static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(const std::string &chain) {
    if (chain == CBaseChainParams::MAIN) {
        return mainParams;
    }

    if (chain == CBaseChainParams::TESTNET) {
        return testNetParams;
    }

    if (chain == CBaseChainParams::REGTEST) {
        return regTestParams;
    }

    throw std::runtime_error(
        strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string &network) {
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

void UpdateRegtestBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime,
                                 int64_t nTimeout) {
    regTestParams.UpdateBIP9Parameters(d, nStartTime, nTimeout);
}
