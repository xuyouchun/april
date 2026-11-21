/*
 *  System.dll
 *
 *  The light synchronization types: each has the lock of its own object, not the gate of the wait handles.
 */

namespace Threading;

////////// ////////// ////////// ////////// //////////

// A count of the threads that can go on, for the threads of the program.
public class SemaphoreSlim : IDisposable
{
    private Int32 __count;
    private Int32 __maximum;

    public SemaphoreSlim(Int32 initialCount) : this(initialCount, 2147483647)
    {
    }

    public SemaphoreSlim(Int32 initialCount, Int32 maxCount)
    {
        if (maxCount < 1 || initialCount < 0 || initialCount > maxCount)
            throw new ArgumentException("the count of the semaphore is not valid");

        __count = initialCount;
        __maximum = maxCount;
    }

    // The threads that can go on now.
    public Int32 CurrentCount => __count;

    // Waits until the count is not 0, takes one.
    public Void Wait()
    {
        Wait(-1);
    }

    // Waits for some milliseconds (-1 is for ever): returns whether a count was taken.
    public Boolean Wait(Int32 millisecondsTimeout)
    {
        Waiting.Check(millisecondsTimeout);

        lock (this)
        {
            Int64 end = Waiting.End(millisecondsTimeout);

            while (__count == 0)
            {
                Int32 left = Waiting.Left(end, millisecondsTimeout);
                if (left == 0)
                    return false;

                Monitor.Wait(this, left);
            }

            __count = __count - 1;
            return true;
        }
    }

    public Boolean Wait(TimeSpan timeout)
    {
        return Wait(Waiting.ToMilliseconds(timeout));
    }

    public Int32 Release()
    {
        return Release(1);
    }

    // Gives some counts back, returns the count before.
    public Int32 Release(Int32 releaseCount)
    {
        if (releaseCount < 1)
            throw new ArgumentException("the count is not positive");

        lock (this)
        {
            Int32 before = __count;
            if (__maximum - __count < releaseCount)
                throw new SemaphoreFullException("the count is over the maximum");

            __count = __count + releaseCount;
            Monitor.PulseAll(this);

            return before;
        }
    }

    public Void Dispose()
    {
    }
}

////////// ////////// ////////// ////////// //////////

// An event of the threads of the program: signaled until Reset is called.
public class ManualResetEventSlim : IDisposable
{
    private Boolean __state;

    public ManualResetEventSlim() : this(false)
    {
    }

    public ManualResetEventSlim(Boolean initialState)
    {
        __state = initialState;
    }

    public Boolean IsSet => __state;

    public Void Set()
    {
        lock (this)
        {
            __state = true;
            Monitor.PulseAll(this);
        }
    }

    public Void Reset()
    {
        lock (this)
            __state = false;
    }

    public Void Wait()
    {
        Wait(-1);
    }

    // Waits for some milliseconds (-1 is for ever): returns whether the event is signaled.
    public Boolean Wait(Int32 millisecondsTimeout)
    {
        Waiting.Check(millisecondsTimeout);

        lock (this)
        {
            Int64 end = Waiting.End(millisecondsTimeout);

            while (!__state)
            {
                Int32 left = Waiting.Left(end, millisecondsTimeout);
                if (left == 0)
                    return false;

                Monitor.Wait(this, left);
            }

            return true;
        }
    }

    public Boolean Wait(TimeSpan timeout)
    {
        return Wait(Waiting.ToMilliseconds(timeout));
    }

    public Void Dispose()
    {
    }
}

////////// ////////// ////////// ////////// //////////

// Waits until a count of signals was made: each Signal takes one, the threads that wait go on when it is 0.
public class CountdownEvent : IDisposable
{
    private Int32 __initial;
    private Int32 __count;

    public CountdownEvent(Int32 initialCount)
    {
        if (initialCount < 0)
            throw new ArgumentException("the count is negative");

        __initial = initialCount;
        __count = initialCount;
    }

    public Int32 InitialCount => __initial;

    public Int32 CurrentCount => __count;

    public Boolean IsSet => __count == 0;

    // Takes one: returns whether the count reached 0.
    public Boolean Signal()
    {
        return Signal(1);
    }

    public Boolean Signal(Int32 signalCount)
    {
        if (signalCount < 1)
            throw new ArgumentException("the count is not positive");

        lock (this)
        {
            if (__count < signalCount)
                throw new InvalidOperationException("the count is less than the signals");

            __count = __count - signalCount;
            if (__count == 0)
                Monitor.PulseAll(this);

            return __count == 0;
        }
    }

    // Adds counts: the event must not be set.
    public Void AddCount()
    {
        AddCount(1);
    }

    public Void AddCount(Int32 signalCount)
    {
        if (!TryAddCount(signalCount))
            throw new InvalidOperationException("the event is set");
    }

    public Boolean TryAddCount()
    {
        return TryAddCount(1);
    }

    public Boolean TryAddCount(Int32 signalCount)
    {
        if (signalCount < 1)
            throw new ArgumentException("the count is not positive");

        lock (this)
        {
            if (__count == 0)
                return false;

            __count = __count + signalCount;
            return true;
        }
    }

    // Sets the count back to the initial one (or another).
    public Void Reset()
    {
        Reset(__initial);
    }

    public Void Reset(Int32 count)
    {
        if (count < 0)
            throw new ArgumentException("the count is negative");

        lock (this)
        {
            __initial = count;
            __count = count;

            if (count == 0)
                Monitor.PulseAll(this);
        }
    }

    public Void Wait()
    {
        Wait(-1);
    }

    // Waits for the count to be 0 for some milliseconds (-1 is for ever): returns whether it is.
    public Boolean Wait(Int32 millisecondsTimeout)
    {
        Waiting.Check(millisecondsTimeout);

        lock (this)
        {
            Int64 end = Waiting.End(millisecondsTimeout);

            while (__count != 0)
            {
                Int32 left = Waiting.Left(end, millisecondsTimeout);
                if (left == 0)
                    return false;

                Monitor.Wait(this, left);
            }

            return true;
        }
    }

    public Boolean Wait(TimeSpan timeout)
    {
        return Wait(Waiting.ToMilliseconds(timeout));
    }

    public Void Dispose()
    {
    }
}

////////// ////////// ////////// ////////// //////////

// The threads of a phase wait for each other: when all the participants have signaled, the phase ends (the action after the
// phase runs, in the thread of the last one) and they go on.
public class Barrier : IDisposable
{
    private Int32 __participants;
    private Int32 __remaining;
    private Int64 __phase;
    private Delegate<Void, Barrier> __postPhase;
    private Boolean __hasPostPhase;

    public Barrier(Int32 participantCount)
    {
        if (participantCount < 0)
            throw new ArgumentException("the count is negative");

        __participants = participantCount;
        __remaining = participantCount;
    }

    public Barrier(Int32 participantCount, Delegate<Void, Barrier> postPhaseAction) : this(participantCount)
    {
        __postPhase = postPhaseAction;
        __hasPostPhase = true;
    }

    public Int32 ParticipantCount => __participants;

    // The participants that have not signaled in the phase.
    public Int32 ParticipantsRemaining => __remaining;

    // The number of the phase: 0 at the beginning, it grows when a phase ends.
    public Int64 CurrentPhaseNumber => __phase;

    // Adds a participant, returns the number of the phase that it joins.
    public Int64 AddParticipant()
    {
        lock (this)
        {
            __participants = __participants + 1;
            __remaining = __remaining + 1;

            return __phase;
        }
    }

    // Removes a participant: the phase ends when it was the last that the others waited for.
    public Void RemoveParticipant()
    {
        lock (this)
        {
            if (__participants == 0)
                throw new InvalidOperationException("there is no participant");

            __participants = __participants - 1;

            if (__remaining > 0)
                __remaining = __remaining - 1;

            if (__remaining == 0 && __participants > 0)
                __EndPhase();
        }
    }

    public Void SignalAndWait()
    {
        SignalAndWait(-1);
    }

    // The participant signals and waits for the others, for some milliseconds (-1 is for ever): returns whether the phase
    // ended; when the time is over the signal is taken back.
    public Boolean SignalAndWait(Int32 millisecondsTimeout)
    {
        Waiting.Check(millisecondsTimeout);

        lock (this)
        {
            if (__participants == 0)
                throw new InvalidOperationException("there is no participant");

            Int64 phase = __phase;
            __remaining = __remaining - 1;

            if (__remaining == 0)
            {
                __EndPhase();
                return true;
            }

            Int64 end = Waiting.End(millisecondsTimeout);

            while (__phase == phase)
            {
                Int32 left = Waiting.Left(end, millisecondsTimeout);
                if (left == 0)
                {
                    __remaining = __remaining + 1;
                    return false;
                }

                Monitor.Wait(this, left);
            }

            return true;
        }
    }

    public Boolean SignalAndWait(TimeSpan timeout)
    {
        return SignalAndWait(Waiting.ToMilliseconds(timeout));
    }

    // The phase ends: the action runs, then the next phase begins and the threads that wait go on.
    private Void __EndPhase()
    {
        try
        {
            if (__hasPostPhase)
                __postPhase(this);
        }
        finally
        {
            __phase = __phase + 1;
            __remaining = __participants;
            Monitor.PulseAll(this);
        }
    }

    public Void Dispose()
    {
    }
}
