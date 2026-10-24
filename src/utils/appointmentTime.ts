import type { Appointment } from '@/types/appointments';

export function isValidAppointmentTime(time: string): boolean {
  return /^([01]\d|2[0-3]):[0-5]\d$/.test(time);
}

export function timesOverlap(startTime: string, otherStartTime: string, duration: number, otherDuration: number): boolean {
  const toMinutes = (value: string): number => {
    const [hour, minute] = value.split(':').map(Number);
    return hour * 60 + minute;
  };
  const start = toMinutes(startTime);
  const otherStart = toMinutes(otherStartTime);
  return start < otherStart + otherDuration && otherStart < start + duration;
}

export function findDoctorConflict(appointments: Appointment[], doctorId: string, date: string, time: string, excludedId?: string): Appointment | undefined {
  if (!doctorId || !date || !isValidAppointmentTime(time)) return undefined;
  return appointments.find((appointment) => appointment.id !== excludedId
    && appointment.doctorId === doctorId
    && appointment.date === date
    && appointment.status !== 'Cancelled'
    && timesOverlap(time, appointment.time, 30, appointment.duration));
}
