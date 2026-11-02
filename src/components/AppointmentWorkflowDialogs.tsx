import { useState } from 'react';
import { appointmentApi } from '@/services/appointmentApi';
import type { Appointment, AppointmentAction, AppointmentQueueState } from '@/types/appointments';

const emptyQueue: AppointmentQueueState = {
  appointmentIds: [],
  front: null,
  empty: true,
};

function appointmentLabel(appointment: Appointment | undefined, id: number) {
  return appointment
    ? `APT-${id} · ${appointment.date} ${appointment.time}`
    : `APT-${id}`;
}

function activityDescription(action: AppointmentAction) {
  switch (action.action) {
    case 'Checked in':
      return 'Patient checked in and added to the processing list.';
    case 'Processed':
      return 'Patient processed from the check-in list.';
    case 'Added to waiting room':
      return 'Patient added to the waiting room.';
    case 'Removed from waiting room':
      return 'Patient removed from the waiting room.';
    case 'Rotated in waiting room':
      return 'Patient moved to the end of the waiting list.';
    default:
      return action.details;
  }
}

export default function AppointmentWorkflowDialogs({
  appointments,
}: {
  appointments: Appointment[];
}) {
  const [dialog, setDialog] = useState<'waiting' | 'activity' | null>(null);
  const [checkIns, setCheckIns] = useState(emptyQueue);
  const [waitingList, setWaitingList] = useState(emptyQueue);
  const [activity, setActivity] = useState<AppointmentAction[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState('');
  const [notice, setNotice] = useState('');

  const loadWaitingList = async () => {
    const [nextCheckIns, nextWaitingList] = await Promise.all([
      appointmentApi.getCheckInQueue(),
      appointmentApi.getWaitingRoomQueue(),
    ]);
    setCheckIns(nextCheckIns);
    setWaitingList(nextWaitingList);
  };

  const loadActivity = async () => {
    setActivity(await appointmentApi.getActionHistory());
  };

  const openDialog = async (target: 'waiting' | 'activity') => {
    setDialog(target);
    setError('');
    setNotice('');
    setLoading(true);
    try {
      if (target === 'waiting')
        await loadWaitingList();
      else
        await loadActivity();
    } catch (loadError) {
      setError(loadError instanceof Error ? loadError.message : 'Unable to load appointment information.');
    } finally {
      setLoading(false);
    }
  };

  const refreshCurrentDialog = async () => {
    if (!dialog)
      return;
    if (dialog === 'waiting')
      await loadWaitingList();
    else
      await loadActivity();
  };

  const performWaitingAction = async (
    operation: () => Promise<{ message: string; appointmentId: number }>,
  ) => {
    setLoading(true);
    setError('');
    setNotice('');
    try {
      const result = await operation();
      setNotice(`${result.message} ${appointmentLabel(
        appointments.find((appointment) => appointment.id === result.appointmentId),
        result.appointmentId,
      )}.`);
      await refreshCurrentDialog();
    } catch (actionError) {
      setError(actionError instanceof Error ? actionError.message : 'Unable to update the waiting list.');
      try {
        await refreshCurrentDialog();
      } catch (refreshError) {
        setError(refreshError instanceof Error ? refreshError.message : 'Unable to refresh the waiting list.');
      }
    } finally {
      setLoading(false);
    }
  };

  const processNextPatient = async () => {
    setLoading(true);
    setError('');
    setNotice('');
    try {
      const result = await appointmentApi.dequeueCheckIn();
      setNotice(`Next patient processed: ${appointmentLabel(
        appointments.find((appointment) => appointment.id === result.appointmentId),
        result.appointmentId,
      )}.`);
      await loadWaitingList();
      setActivity(await appointmentApi.getActionHistory());
    } catch (actionError) {
      setError(actionError instanceof Error ? actionError.message : 'Unable to process the next patient.');
      try {
        await loadWaitingList();
      } catch (refreshError) {
        setError(refreshError instanceof Error ? refreshError.message : 'Unable to refresh the waiting list.');
      }
    } finally {
      setLoading(false);
    }
  };

  const runWaitingAction = (operation: () => Promise<{
    success: boolean;
    message: string;
    appointmentId: number;
  }>) => performWaitingAction(operation);

  return (
    <>
      <button
        type="button"
        onClick={() => void openDialog('waiting')}
        className="flex h-10 items-center justify-center rounded-lg border border-[#d6e4df] px-3 text-[12px] font-semibold text-[#3b7264] transition hover:bg-[#eef7f3]"
      >
        Waiting list
      </button>
      <button
        type="button"
        onClick={() => void openDialog('activity')}
        className="flex h-10 items-center justify-center rounded-lg border border-[#d6e4df] px-3 text-[12px] font-semibold text-[#3b7264] transition hover:bg-[#eef7f3]"
      >
        Appointment activity
      </button>

      {dialog && (
        <div
          className="fixed inset-0 z-50 flex items-center justify-center bg-[#173b35]/40 p-4"
          onMouseDown={(event) => {
            if (event.target === event.currentTarget)
              setDialog(null);
          }}
        >
          <section
            role="dialog"
            aria-modal="true"
            aria-labelledby="appointment-workflow-title"
            className="max-h-[85vh] w-full max-w-2xl overflow-y-auto rounded-xl border border-[#e0ebe8] bg-white p-5 shadow-2xl"
          >
            <div className="flex items-start justify-between gap-4">
              <div>
                <h3 id="appointment-workflow-title" className="font-display text-[19px] font-bold text-[#19483f]">
                  {dialog === 'waiting' ? 'Patient waiting list' : 'Appointment activity'}
                </h3>
                <p className="mt-1 text-[12px] text-[#738480]">
                  {dialog === 'waiting'
                    ? 'Review checked-in patients and manage the waiting list.'
                    : 'Recent scheduling and patient-flow activity.'}
                </p>
              </div>
              <button
                type="button"
                aria-label="Close dialog"
                onClick={() => setDialog(null)}
                className="rounded-md px-2 py-1 text-[20px] leading-none text-[#738480] hover:bg-[#f1f8f5]"
              >
                ×
              </button>
            </div>

            {error && <div role="alert" className="mt-4 rounded-lg border border-[#f0d2c9] bg-[#fff7f3] px-3 py-2 text-[12px] text-[#a45d4d]">{error}</div>}
            {notice && <div role="status" className="mt-4 rounded-lg border border-[#cce8da] bg-[#effaf4] px-3 py-2 text-[12px] text-[#2c765b]">{notice}</div>}

            {loading ? (
              <div role="status" className="py-10 text-center text-[13px] text-[#738480]">Loading appointment information…</div>
            ) : dialog === 'waiting' ? (
              <div className="mt-5 space-y-5">
                <section>
                  <div className="mb-2 flex items-center justify-between gap-3">
                    <div>
                      <h4 className="text-[13px] font-semibold text-[#31554d]">Checked-in patients</h4>
                      <p className="text-[11px] text-[#8a9996]">Patients are processed in check-in order.</p>
                    </div>
                    <button
                      type="button"
                      disabled={checkIns.empty || loading}
                      onClick={() => void processNextPatient()}
                      className="rounded-lg bg-[#176153] px-3 py-2 text-[11px] font-semibold text-white disabled:opacity-50"
                    >
                      Process next patient
                    </button>
                  </div>
                  <div className="divide-y divide-[#eef3f1] rounded-lg border border-[#e4ecea]">
                    {checkIns.appointmentIds.length
                      ? checkIns.appointmentIds.map((id, index) => (
                        <div key={id} className="flex items-center justify-between gap-3 px-3 py-2.5 text-[12px] text-[#49635e]">
                          <span>{index === 0 ? 'Next · ' : ''}{appointmentLabel(appointments.find((item) => item.id === id), id)}</span>
                        </div>
                      ))
                      : <p className="px-3 py-4 text-[12px] text-[#8a9996]">No patients are waiting to be processed.</p>}
                  </div>
                </section>

                <section>
                  <div className="mb-2">
                    <h4 className="text-[13px] font-semibold text-[#31554d]">Waiting room</h4>
                    <p className="text-[11px] text-[#8a9996]">Move a patient to the back of the list or remove them when ready.</p>
                  </div>
                  <div className="divide-y divide-[#eef3f1] rounded-lg border border-[#e4ecea]">
                    {waitingList.appointmentIds.length
                      ? waitingList.appointmentIds.map((id, index) => (
                        <div key={id} className="flex flex-wrap items-center justify-between gap-2 px-3 py-2.5">
                          <span className="text-[12px] text-[#49635e]">{index === 0 ? 'Next · ' : ''}{appointmentLabel(appointments.find((item) => item.id === id), id)}</span>
                          <div className="flex gap-2">
                            <button
                              type="button"
                              disabled={waitingList.appointmentIds.length < 2 || loading}
                              onClick={() => void runWaitingAction(() => appointmentApi.rotateWaitingRoom())}
                              className="rounded-md border border-[#dce6e3] px-2.5 py-1.5 text-[10px] font-semibold text-[#3b7264] disabled:opacity-50"
                            >
                              Move to back
                            </button>
                            <button
                              type="button"
                              disabled={loading}
                              onClick={() => void runWaitingAction(() => appointmentApi.dequeueWaitingRoom())}
                              className="rounded-md border border-[#eadbd6] px-2.5 py-1.5 text-[10px] font-semibold text-[#a45d4d] disabled:opacity-50"
                            >
                              Remove
                            </button>
                          </div>
                        </div>
                      ))
                      : <p className="px-3 py-4 text-[12px] text-[#8a9996]">The waiting room is empty.</p>}
                  </div>
                  <p className="mt-2 text-right text-[10px] text-[#8a9996]">
                    {waitingList.appointmentIds.length} of {waitingList.capacity ?? 10} places in use
                  </p>
                </section>
              </div>
            ) : (
              <div className="mt-5 divide-y divide-[#eef3f1] rounded-lg border border-[#e4ecea]">
                {activity.length
                  ? activity.map((item, index) => (
                    <div key={`${item.timestamp}-${item.appointmentId}-${index}`} className="px-3 py-3">
                      <p className="text-[12px] font-medium text-[#31554d]">
                        {item.action} · {appointmentLabel(appointments.find((appointment) => appointment.id === item.appointmentId), item.appointmentId)}
                      </p>
                      <p className="mt-1 text-[11px] text-[#738480]">{activityDescription(item)}</p>
                      <p className="mt-1 text-[10px] text-[#9aa7a4]">{item.timestamp}</p>
                    </div>
                  ))
                  : <p className="px-3 py-4 text-[12px] text-[#8a9996]">No appointment activity is available.</p>}
              </div>
            )}
          </section>
        </div>
      )}
    </>
  );
}
