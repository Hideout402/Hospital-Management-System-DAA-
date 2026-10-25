import { createContext, useContext, useMemo, useState, type ReactNode } from 'react';
import { doctors, initialAppointments, patients } from '@/data/mockAppointments';
import type { Appointment, AppointmentDraft, Doctor, Patient } from '@/types/appointments';

interface AppointmentContextValue {
  appointments: Appointment[];
  patients: Patient[];
  doctors: Doctor[];
  saveAppointment: (draft: AppointmentDraft, id?: string) => void;
  cancelAppointment: (id: string) => void;
  getPatient: (id: string) => Patient;
  getDoctor: (id: string) => Doctor;
}

const AppointmentContext = createContext<AppointmentContextValue | undefined>(undefined);

export function AppointmentProvider({ children }: { children: ReactNode }) {
  const [appointments, setAppointments] = useState<Appointment[]>(initialAppointments);

  const value = useMemo<AppointmentContextValue>(() => ({
    appointments,
    patients,
    doctors,
    saveAppointment: (draft, id) => setAppointments((current) => id
      ? current.map((appointment) => appointment.id === id ? { ...appointment, ...draft } : appointment)
      : [{ ...draft, id: `APT-2026-${String(current.length + 1).padStart(3, '0')}`, duration: 30 }, ...current]),
    cancelAppointment: (id) => setAppointments((current) => current.map((appointment) => appointment.id === id ? { ...appointment, status: 'Cancelled' } : appointment)),
    getPatient: (id) => patients.find((patient) => patient.id === id) ?? patients[0],
    getDoctor: (id) => doctors.find((doctor) => doctor.id === id) ?? doctors[0],
  }), [appointments]);

  return <AppointmentContext.Provider value={value}>{children}</AppointmentContext.Provider>;
}

// eslint-disable-next-line react-refresh/only-export-components
export function useAppointments() {
  const context = useContext(AppointmentContext);
  if (!context) throw new Error('useAppointments must be used inside AppointmentProvider');
  return context;
}
