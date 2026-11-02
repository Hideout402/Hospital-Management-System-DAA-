import { createContext, useCallback, useContext, useEffect, useMemo, useState, type ReactNode } from 'react';
import { appointmentApi } from '@/services/appointmentApi';
import type { Appointment, AppointmentDraft, Doctor, Patient } from '@/types/appointments';

interface AppointmentContextValue {
  appointments: Appointment[];
  patients: Patient[];
  doctors: Doctor[];
  loading: boolean;
  error: string;
  refresh: () => Promise<void>;
  saveAppointment: (draft: AppointmentDraft, id?: number) => Promise<void>;
  cancelAppointment: (id: number) => Promise<void>;
  getPatient: (id: number) => Patient;
  getDoctor: (id: number) => Doctor;
}

const AppointmentContext = createContext<AppointmentContextValue | undefined>(undefined);

export function AppointmentProvider({ children }: { children: ReactNode }) {
  const [appointments, setAppointments] = useState<Appointment[]>([]);
  const [patients, setPatients] = useState<Patient[]>([]);
  const [doctors, setDoctors] = useState<Doctor[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState('');

  const loadData = useCallback(() => Promise.all([
    appointmentApi.getAll(),
    appointmentApi.getOptions(),
  ]), []);

  const load = useCallback(async () => {
    try {
      const [nextAppointments, options] = await loadData();
      setAppointments(nextAppointments);
      setPatients(options.patients);
      setDoctors(options.doctors);
      setError('');
    } catch (requestError) {
      setError(requestError instanceof Error ? requestError.message : 'Unable to load appointments.');
    } finally {
      setLoading(false);
    }
  }, [loadData]);

  const refresh = useCallback(async () => {
    setLoading(true);
    setError('');
    await load();
  }, [load]);

  useEffect(() => {
    let active = true;
    void loadData()
      .then(([nextAppointments, options]) => {
        if (!active) return;
        setAppointments(nextAppointments);
        setPatients(options.patients);
        setDoctors(options.doctors);
        setError('');
      })
      .catch((requestError: unknown) => {
        if (active) {
          setError(requestError instanceof Error ? requestError.message : 'Unable to load appointments.');
        }
      })
      .finally(() => {
        if (active) setLoading(false);
      });

    return () => {
      active = false;
    };
  }, [loadData]);

  const saveAppointment = useCallback(async (draft: AppointmentDraft, id?: number) => {
    const saved = id === undefined
      ? (await appointmentApi.create(draft)).appointment
      : (await appointmentApi.update(id, draft)).appointment;
    setAppointments((current) => id === undefined
      ? [saved, ...current]
      : current.map((appointment) => appointment.id === id ? saved : appointment));
    setError('');
  }, []);

  const cancelAppointment = useCallback(async (id: number) => {
    const result = await appointmentApi.cancel(id);
    setAppointments((current) => current.map((appointment) =>
      appointment.id === id ? result.appointment : appointment));
    setError('');
  }, []);

  const value = useMemo<AppointmentContextValue>(() => ({
    appointments,
    patients,
    doctors,
    loading,
    error,
    refresh,
    saveAppointment,
    cancelAppointment,
    getPatient: (id) => patients.find((patient) => patient.id === id) ?? {
      id,
      name: `Unknown patient (ID ${id})`,
      age: 0,
      phone: '',
      gender: '',
    },
    getDoctor: (id) => doctors.find((doctor) => doctor.id === id) ?? {
      id,
      name: `Unknown doctor (ID ${id})`,
      specialty: '',
      department: '',
      fee: 0,
    },
  }), [appointments, patients, doctors, loading, error, refresh, saveAppointment, cancelAppointment]);

  return <AppointmentContext.Provider value={value}>{children}</AppointmentContext.Provider>;
}

// eslint-disable-next-line react-refresh/only-export-components
export function useAppointments() {
  const context = useContext(AppointmentContext);
  if (!context) throw new Error('useAppointments must be used inside AppointmentProvider');
  return context;
}
