/* Copyright 2012 Geehalel (geehalel AT gmail DOT com) */
/* This file is part of the Skywatcher Protocol INDI driver.

    The Skywatcher Protocol INDI driver is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The Skywatcher Protocol INDI driver is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the Skywatcher Protocol INDI driver.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "config.h"
#include "skywatcher.h"
#ifdef WITH_ALIGN_GEEHALEL
#include "align/align.h"
#endif
#include "simulator/simulator.h"
#ifdef WITH_SCOPE_LIMITS
#include "scope-limits/scope-limits.h"
#endif

#include <inditelescope.h>
#include <indiguiderinterface.h>

#include <libnova/ln_types.h>

typedef struct SyncData
{
    double lst, jd;
    double targetRA, targetDEC;
    double telescopeRA, telescopeDEC;
    double deltaRA, deltaDEC;
    uint32_t targetRAEncoder, targetDECEncoder;
    uint32_t telescopeRAEncoder, telescopeDECEncoder;
    long deltaRAEncoder, deltaDECEncoder;
} SyncData;
#ifdef WITH_ALIGN

#include <alignment/AlignmentSubsystemForDrivers.h>

class EQMod : public INDI::Telescope, public INDI::GuiderInterface, INDI::AlignmentSubsystem::AlignmentSubsystemForDrivers
#else
class EQMod : public INDI::Telescope, public INDI::GuiderInterface
#endif
{
    protected:
        //  private:
        Skywatcher *mount;

        uint32_t currentRAEncoder, zeroRAEncoder, totalRAEncoder;
        uint32_t currentDEEncoder, zeroDEEncoder, totalDEEncoder;

        uint32_t homeRAEncoder, parkRAEncoder;
        uint32_t homeDEEncoder, parkDEEncoder;

        double currentRA, currentHA;
        double currentDEC;
        double alignedRA, alignedDEC;
        double ghalignedRA, ghalignedDEC;
        double targetRA;
        double targetDEC;

#ifdef WITH_ALIGN_GEEHALEL
        Align *align;
#endif

        int last_motion_ns;
        int last_motion_ew;

        INDI::IEquatorialCoordinates lnradec;
        INDI::IHorizontalCoordinates lnaltaz;

        /* Time variables */
        struct tm utc;
        struct ln_date lndate;
        struct timeval lasttimeupdate;
        struct timespec lastclockupdate;
        double juliandate;

        int GuideTimerNS;
        int GuideTimerWE;

        // INumber *GuideRateN                        = nullptr;
        INDI::PropertyNumber   GuideRateNP         {INDI::Property()};
        INDI::PropertyText     MountInformationTP  {INDI::Property()};
        INDI::PropertyNumber   SteppersNP          {INDI::Property()};
        INDI::PropertyNumber   CurrentSteppersNP   {INDI::Property()};
        INDI::PropertyNumber   PeriodsNP           {INDI::Property()};
        INDI::PropertyNumber   JulianNP            {INDI::Property()};
        INDI::PropertyNumber   TimeLSTNP           {INDI::Property()};
        INDI::PropertyLight    RAStatusLP          {INDI::Property()};
        INDI::PropertyLight    DEStatusLP          {INDI::Property()};
        INDI::PropertyNumber   SlewSpeedsNP        {INDI::Property()};
        INDI::PropertySwitch   HemisphereSP        {INDI::Property()};
        INDI::PropertySwitch   TrackDefaultSP      {INDI::Property()};
        INDI::PropertyNumber   HorizontalCoordNP   {INDI::Property()};
        INDI::PropertyNumber   StandardSyncNP      {INDI::Property()};
        INDI::PropertyNumber   StandardSyncPointNP {INDI::Property()};
        INDI::PropertyNumber   SyncPolarAlignNP    {INDI::Property()};
        INDI::PropertySwitch   SyncManageSP        {INDI::Property()};
        INDI::PropertySwitch   ReverseDECSP        {INDI::Property()};
        INDI::PropertySwitch   TargetPierSideSP    {INDI::Property()};
        INDI::PropertyNumber   BacklashNP          {INDI::Property()};
        INDI::PropertySwitch   UseBacklashSP       {INDI::Property()};
        INDI::PropertyNumber   LEDBrightnessNP     {INDI::Property()};
#if defined WITH_ALIGN && defined WITH_ALIGN_GEEHALEL
        ISwitch AlignMethodS[2];
        ISwitchVectorProperty AlignMethodSP;
#endif
#if defined WITH_ALIGN || defined WITH_ALIGN_GEEHALEL
        INDI::PropertySwitch   AlignSyncModeSP     {INDI::Property()};
#endif
        INDI::PropertySwitch   AutoHomeSP          {INDI::Property()};
        INDI::PropertySwitch   AuxEncoderSP        {INDI::Property()};
        INDI::PropertyNumber   AuxEncoderNP        {INDI::Property()};

        INDI::PropertySwitch   ST4GuideRateNSSP    {INDI::Property()};
        INDI::PropertySwitch   ST4GuideRateWESP    {INDI::Property()};

        INDI::PropertySwitch   PPECTrainingSP      {INDI::Property()};
        INDI::PropertySwitch   PPECSP              {INDI::Property()};

        INDI::PropertySwitch   SNAPPORT1SP         {INDI::Property()};
        INDI::PropertySwitch   SNAPPORT2SP         {INDI::Property()};

        INumber *MinPulseN                   = nullptr;
        INumber *MinPulseTimerN              = nullptr;
        INDI::PropertyNumber   PulseLimitsNP       {INDI::Property()};

        enum Hemisphere
        {
            NORTH = 0,
            SOUTH = 1
        };

        typedef struct GotoParams
        {
            double ratarget, detarget, racurrent, decurrent;
            uint32_t ratargetencoder, detargetencoder, racurrentencoder, decurrentencoder;
            uint32_t limiteast, limitwest;
            unsigned int iterative_count;
            bool checklimits, outsidelimits, completed;
            TelescopePierSide pier_side;
        } GotoParams;

        Hemisphere Hemisphere;
        bool RAInverted, DEInverted;
        TelescopePierSide TargetPier = PIER_UNKNOWN;
        GotoParams gotoparams;
        SyncData syncdata, syncdata2;

        double tpa_alt, tpa_az;

        void EncodersToRADec(uint32_t rastep, uint32_t destep, double lst, double *ra, double *de, double *ha,
                             TelescopePierSide *pierSide);
        double EncoderToHours(uint32_t destep, uint32_t initdestep, uint32_t totalrastep, enum Hemisphere h);
        double EncoderToDegrees(uint32_t destep, uint32_t initdestep, uint32_t totalrastep,
                                enum Hemisphere h);
        double EncoderFromHour(double hour, uint32_t initstep, uint32_t totalstep, enum Hemisphere h);
        double EncoderFromRA(double ratarget, TelescopePierSide p, double lst, uint32_t initstep, uint32_t totalstep,
                             enum Hemisphere h);
        double EncoderFromDegree(double degree, uint32_t initstep, uint32_t totalstep,
                                 enum Hemisphere h);
        double EncoderFromDec(double detarget, TelescopePierSide p, uint32_t initstep, uint32_t totalstep,
                              enum Hemisphere h);
        void EncoderTarget(GotoParams *g);
        void SetSouthernHemisphere(bool southern);
        void UpdateDEInverted();
        double GetRATrackRate();
        double GetDETrackRate();
        double GetDefaultRATrackRate();
        double GetDefaultDETrackRate();
        static void timedguideNSCallback(void *userpointer);
        static void timedguideWECallback(void *userpointer);
        double GetRASlew();
        double GetDESlew();
        bool gotoInProgress();

        bool loadProperties();

        void setStepperSimulation(bool enable);

        void computePolarAlign(SyncData s1, SyncData s2, double lat, double *tpaalt, double *tpaaz);
        void starPolarAlign(double lst, double ra, double dec, double theta, double gamma, double *tra, double *tdec);
#if defined WITH_ALIGN || defined WITH_ALIGN_GEEHALEL
        bool isStandardSync();
#endif
        // Autohoming for EQ8
        int ah_confirm_timeout;
        bool ah_bSlewingUp_RA, ah_bSlewingUp_DE;
        uint32_t ah_iPosition_RA, ah_iPosition_DE;
        int ah_iChanges;
        bool ah_bIndexChanged_RA, ah_bIndexChanged_DE;
        uint32_t ah_sHomeIndexPosition_RA, ah_sHomeIndexPosition_DE;
        int ah_waitRA, ah_waitDE;

        // save PPEC status when guiding
        bool restartguidePPEC;

        // One bit for each axis
        uint8_t pulseInProgress;

    public:
        EQMod();
        virtual ~EQMod();

        virtual const char *getDefaultName();
        virtual bool Handshake();
        virtual bool Disconnect();
        virtual void TimerHit();
        virtual bool ReadScopeStatus();
        virtual bool initProperties();
        virtual bool updateProperties();
        virtual void ISGetProperties(const char *dev);
        virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n);
        virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n);
        virtual bool ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n);
#ifdef WITH_ALIGN
        virtual bool ISNewBLOB(const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[],
                               char *formats[], char *names[], int n);
#endif
        virtual bool MoveNS(INDI_DIR_NS dir, TelescopeMotionCommand command);
        virtual bool MoveWE(INDI_DIR_WE dir, TelescopeMotionCommand command);
        virtual bool Abort();

        virtual IPState GuideNorth(uint32_t ms);
        virtual IPState GuideSouth(uint32_t ms);
        virtual IPState GuideEast(uint32_t ms);
        virtual IPState GuideWest(uint32_t ms);

        bool Goto(double ra, double dec);
        bool Park();
        bool UnPark();
        bool SetCurrentPark();
        bool SetDefaultPark();
        bool Sync(double ra, double dec);

        // Call first time EQMod is run
        void saveInitialParkPosition();

        // Called when there is an unrecoverable tty error
        void abnormalDisconnect();
        static void abnormalDisconnectCallback(void *userpointer);

        // Tracking
        bool SetTrackMode(uint8_t mode);
        bool SetTrackRate(double raRate, double deRate);
        bool SetTrackEnabled(bool enabled);

        virtual bool saveConfigItems(FILE *fp);

        bool updateTime(ln_date *lndate_utc, double utc_offset);
        bool updateLocation(double latitude, double longitude, double elevation);

        double getLongitude();
        double getLatitude();
        double getJulianDate();
        double getLst(double jd, double lng);

        EQModSimulator *simulator;

#ifdef WITH_SCOPE_LIMITS
        HorizonLimits *horizon;
#endif
        // AutoHoming for EQ8
        static const TelescopeStatus SCOPE_AUTOHOMING = static_cast<TelescopeStatus>(SCOPE_PARKED + 1);
        enum AutoHomeStatus
        {
            AUTO_HOME_IDLE,
            AUTO_HOME_CONFIRM,
            AUTO_HOME_WAIT_PHASE1,
            AUTO_HOME_WAIT_PHASE2,
            AUTO_HOME_WAIT_PHASE3,
            AUTO_HOME_WAIT_PHASE4,
            AUTO_HOME_WAIT_PHASE5,
            AUTO_HOME_WAIT_PHASE6
        };
        AutoHomeStatus AutohomeState;

        int DBG_SCOPE_STATUS {0};
        int DBG_COMM {0};
        int DBG_MOUNT {0};
};
