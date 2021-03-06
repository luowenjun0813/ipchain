﻿#include <iostream>
#include "../util.h"
#include "ConsensusAccount.h"

CConsensusAccount::CConsensusAccount(const uint160& pubkeyhash, CAmount& JoinIPC, const uint256& hash, int64_t credit)
{
	mPubicKey160hash =	pubkeyhash;
	mSortValue = uint256();
	miCredit = credit;
	txhash = hash;
	creditBackup = 0;
	mJoinIPC = JoinIPC;
}

CConsensusAccount::CConsensusAccount(const uint160& pubkeyhash, CAmount& JoinIPC, int64_t credit)
{
	mPubicKey160hash = pubkeyhash;
	mSortValue = uint256();
	miCredit = credit;
	txhash = uint256();
	creditBackup = 0;
	mJoinIPC = JoinIPC;
}

CConsensusAccount::CConsensusAccount(const uint160& pubkeyhash)
{
	mPubicKey160hash = pubkeyhash;
	mSortValue = uint256();
	miCredit = 0;
	mJoinIPC = 0;
}

CConsensusAccount::CConsensusAccount()
{
	miCredit = 0;
	creditBackup = 0;					   
	mJoinIPC = 0;
}


CConsensusAccount::CConsensusAccount(const CConsensusAccount* rhs) {
	this->mPubicKey160hash = rhs->mPubicKey160hash;
	this->mSortValue = rhs->mSortValue;
	this->miCredit = rhs->miCredit;
	this->mJoinIPC = rhs->mJoinIPC;
}

CConsensusAccount::~CConsensusAccount()
{
}

CConsensusAccount& CConsensusAccount::operator= (const CConsensusAccount& rhs) {
	if (this == &rhs){
		return *this;
	}

	this->mPubicKey160hash	= rhs.mPubicKey160hash;
	this->mSortValue		= rhs.mSortValue;
	this->miCredit			= rhs.miCredit;
	this->mJoinIPC			= rhs.mJoinIPC;

	return *this;
}

uint256 CConsensusAccount::getSortValue(){
	return mSortValue;
}
void CConsensusAccount::setSortValue(uint256 uSortValue) {
	 mSortValue = uSortValue;
}
uint160 CConsensusAccount::getPubicKey160hash() {
	return  mPubicKey160hash;
}

void CConsensusAccount::debugOut() {
	LogPrintf("%s ", mPubicKey160hash.GetHex().c_str());
}

CAmount CConsensusAccount::getJoinIPC() {
	return mJoinIPC;
}

void CConsensusAccount::backupCredit() { 
	LogPrintf("[CConsensusAccount::backupCredit] Back up the current credit value %d to the backup value, override the original backup value %d\n", 
		      miCredit, creditBackup);
	creditBackup = miCredit; 
};
void CConsensusAccount::revertCredit() {
	LogPrintf("[CConsensusAccount::revertCredit] From the backup value %d recovery of credit value，Cover the original credit value %d\n", creditBackup, miCredit);
	miCredit = creditBackup;
};


CLocalAccount::CLocalAccount()
{
	std::string strPublicKey;
	
	CDpocInfo::Instance().getLocalAccoutVar(strPublicKey);
	pubicKey160hash.SetHex(strPublicKey);
}

bool CLocalAccount::IsNull()
{
	if (pubicKey160hash.IsNull())
	{
		return true;
	}
	return false;
}

int CLocalAccount::Get160Hash(uint160 &hash160)
{
	hash160 = pubicKey160hash;
	return 0;
}

int CLocalAccount::Set160Hash()
{
	std::string strPublicKey;

	CDpocInfo::Instance().getLocalAccoutVar(strPublicKey);
	pubicKey160hash.SetHex(strPublicKey);
	return 0;
}

CLocalAccount::CLocalAccount(const CLocalAccount& loaclAccount)
{
	pubicKey160hash = loaclAccount.pubicKey160hash;
}

