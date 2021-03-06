#include "TestLatency.h"

#include "IPDLUnitTests.h"      // fail etc.

// A ping/pong trial takes O(100us) or more, so if we don't have 10us
// resolution or better, the results will not be terribly useful
static const double kTimingResolutionCutoff = 0.00001; // 10us

namespace mozilla {
namespace _ipdltest {

//-----------------------------------------------------------------------------
// parent

TestLatencyParent::TestLatencyParent() :
    mStart(),
    mPPTimeTotal(),
    mPP5TimeTotal(),
    mRpcTimeTotal(),
    mPPTrialsToGo(NR_TRIALS),
    mPP5TrialsToGo(NR_TRIALS),
    mSpamsToGo(NR_TRIALS)
{
    MOZ_COUNT_CTOR(TestLatencyParent);
}

TestLatencyParent::~TestLatencyParent()
{
    MOZ_COUNT_DTOR(TestLatencyParent);
}

void
TestLatencyParent::Main()
{
    TimeDuration resolution = TimeDuration::Resolution();
    if (resolution.ToSeconds() > kTimingResolutionCutoff) {
        puts("  (skipping TestLatency, timing resolution is too poor)");
        Close();
        return;
    }

    printf("  timing resolution: %g seconds\n",
           resolution.ToSecondsSigDigits());

    if (mozilla::ipc::LoggingEnabled())
        NS_RUNTIMEABORT("you really don't want to log all IPC messages during this test, trust me");

    PingPongTrial();
}

void
TestLatencyParent::PingPongTrial()
{
    mStart = TimeStamp::Now();
    if (!SendPing())
        fail("sending Ping()");
}

void
TestLatencyParent::Ping5Pong5Trial()
{
    mStart = TimeStamp::Now();

    if (!SendPing5() ||
        !SendPing5() ||
        !SendPing5() ||
        !SendPing5() ||
        !SendPing5())
        fail("sending Ping5()");
}

bool
TestLatencyParent::RecvPong()
{
    TimeDuration thisTrial = (TimeStamp::Now() - mStart);
    mPPTimeTotal += thisTrial;

    if (0 == (mPPTrialsToGo % 1000))
        printf("  PP trial %d: %g\n",
               mPPTrialsToGo, thisTrial.ToSecondsSigDigits());

    if (--mPPTrialsToGo > 0)
        PingPongTrial();
    else
        Ping5Pong5Trial();
    return true;
}

bool
TestLatencyParent::RecvPong5()
{
    if (PTestLatency::PING5 != state())
        return true;

    TimeDuration thisTrial = (TimeStamp::Now() - mStart);
    mPP5TimeTotal += thisTrial;

    if (0 == (mPP5TrialsToGo % 1000))
        printf("  PP5 trial %d: %g\n",
               mPP5TrialsToGo, thisTrial.ToSecondsSigDigits());

    if (0 < --mPP5TrialsToGo)
        Ping5Pong5Trial();
    else
        RpcTrials();

    return true;
}

void
TestLatencyParent::RpcTrials()
{
    TimeStamp start = TimeStamp::Now();
    for (int i = 0; i < NR_TRIALS; ++i) {
        if (!CallRpc())
            fail("can't call Rpc()");
        if (0 == (i % 1000))
            printf("  Rpc trial %d\n", i);
    }
    mRpcTimeTotal = (TimeStamp::Now() - start);

    SpamTrial();
}

void
TestLatencyParent::SpamTrial()
{
    TimeStamp start = TimeStamp::Now();
    for (int i = 0; i < NR_SPAMS - 1; ++i) {
        if (!SendSpam())
            fail("sending Spam()");
        if (0 == (i % 10000))
            printf("  Spam trial %d\n", i);
    }

    // Synchronize with the child process to ensure all messages have
    // been processed.  This adds the overhead of a reply message from
    // child-->here, but should be insignificant compared to >>
    // NR_SPAMS.
    if (!CallSynchro())
        fail("calling Synchro()");

    mSpamTimeTotal = (TimeStamp::Now() - start);

    Exit();
}

void
TestLatencyParent::Exit()
{
    Close();
}

//-----------------------------------------------------------------------------
// child

TestLatencyChild::TestLatencyChild()
{
    MOZ_COUNT_CTOR(TestLatencyChild);
}

TestLatencyChild::~TestLatencyChild()
{
    MOZ_COUNT_DTOR(TestLatencyChild);
}

bool
TestLatencyChild::RecvPing()
{
    SendPong();
    return true;
}

bool
TestLatencyChild::RecvPing5()
{
    if (PTestLatency::PONG1 != state())
        return true;

    if (!SendPong5() ||
        !SendPong5() ||
        !SendPong5() ||
        !SendPong5() ||
        !SendPong5())
        fail("sending Pong5()");

    return true;
}

bool
TestLatencyChild::AnswerRpc()
{
    return true;
}

bool
TestLatencyChild::RecvSpam()
{
    // no-op
    return true;
}

bool
TestLatencyChild::AnswerSynchro()
{
    return true;
}

} // namespace _ipdltest
} // namespace mozilla
