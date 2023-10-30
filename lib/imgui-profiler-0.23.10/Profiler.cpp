
#include "ImProfiler.h"

#if WITH_PROFILING

#include <string>
#include <windows.h>

CPUProfiler gCPUProfiler;


//-----------------------------------------------------------------------------
// [SECTION] CPU Profiler
//-----------------------------------------------------------------------------


void CPUProfiler::Initialize(uint32 historySize, uint32 maxEvents)
{
	Shutdown();

	m_pEventData = new EventData[historySize];
	m_HistorySize = historySize;

	for (uint32 i = 0; i < historySize; ++i)
		m_pEventData[i].Events.resize(maxEvents);
}


void CPUProfiler::Shutdown()
{
	delete[] m_pEventData;
}


void CPUProfiler::BeginEvent(const char* pName, const char* pFilePath, uint32 lineNumber)
{
	if (m_EventCallback.OnEventBegin)
		m_EventCallback.OnEventBegin(pName, m_EventCallback.pUserData);

	if (m_Paused)
		return;

	EventData& data = GetData();
	uint32 newIndex = data.NumEvents.fetch_add(1);
	assert(newIndex < data.Events.size());

	TLS& tls = GetTLS();

	EventData::Event& newEvent = data.Events[newIndex];
	newEvent.Depth = tls.EventStack.GetSize();
	newEvent.ThreadIndex = tls.ThreadIndex;
	newEvent.pName = data.Allocator.String(pName);
	newEvent.pFilePath = pFilePath;
	newEvent.LineNumber = lineNumber;
	QueryPerformanceCounter((LARGE_INTEGER*)(&newEvent.TicksBegin));

	tls.EventStack.Push() = newIndex;
}


// End and pop the last pushed event on the current thread
void CPUProfiler::EndEvent()
{
	if (m_EventCallback.OnEventEnd)
		m_EventCallback.OnEventEnd(m_EventCallback.pUserData);

	if (m_Paused)
		return;

	EventData::Event& event = GetData().Events[GetTLS().EventStack.Pop()];
	QueryPerformanceCounter((LARGE_INTEGER*)(&event.TicksEnd));
}


void CPUProfiler::Tick()
{
	m_Paused = m_QueuedPaused;
	if (m_Paused)
		return;

	if (m_FrameIndex)
		EndEvent();

	// Check if all threads have ended all open sample events
	for (auto& threadData : m_ThreadData)
		assert(threadData.pTLS->EventStack.GetSize() == 0);

	// Sort the events by thread and group by thread
	EventData& frame = GetData();
	std::vector<EventData::Event>& events = frame.Events;
	std::sort(events.begin(), events.begin() + frame.NumEvents, [](const EventData::Event& a, const EventData::Event& b)
		{
			return a.ThreadIndex < b.ThreadIndex;
		});

	URange eventRange(0, 0);
	for (uint32 threadIndex = 0; threadIndex < (uint32)m_ThreadData.size(); ++threadIndex)
	{
		while (threadIndex < events[eventRange.Begin].ThreadIndex)
			eventRange.Begin++;
		eventRange.End = eventRange.Begin;
		while (events[eventRange.End].ThreadIndex == threadIndex && eventRange.End < frame.NumEvents)
			++eventRange.End;

		frame.EventsPerThread[threadIndex] = Span<const EventData::Event>(&events[eventRange.Begin], eventRange.End - eventRange.Begin);
		eventRange.Begin = eventRange.End;
	}

	++m_FrameIndex;

	EventData& newData = GetData();
	newData.Allocator.Reset();
	newData.NumEvents = 0;

	BeginEvent("CPU Frame");
}


void CPUProfiler::RegisterThread(const char* pName)
{
	TLS& tls = GetTLSUnsafe();
	assert(!tls.IsInitialized);
	tls.IsInitialized = true;
	std::scoped_lock lock(m_ThreadDataLock);
	tls.ThreadIndex = (uint32)m_ThreadData.size();
	ThreadData& data = m_ThreadData.emplace_back();


	// If the name is not provided, retrieve it using GetThreadDescription()

	data.ThreadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
	strcpy_s(data.Name, std::size(data.Name), pName ? pName : std::to_string(data.ThreadID).c_str());

	data.pTLS = &tls;
	data.Index = (uint32)m_ThreadData.size() - 1;

	for (uint32 i = 0; i < m_HistorySize; ++i)
		m_pEventData[i].EventsPerThread.resize(m_ThreadData.size());
}

#endif
