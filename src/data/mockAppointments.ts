import type { Appointment, Doctor, Patient } from '@/types/appointments';

export const patients: Patient[] = [
  { id: 'PT-10482', name: 'Olivia Bennett', age: 34, phone: '+1 (555) 014-2841', gender: 'Female' },
  { id: 'PT-10483', name: 'James Wilson', age: 58, phone: '+1 (555) 014-9210', gender: 'Male' },
  { id: 'PT-10484', name: 'Sophia Martinez', age: 27, phone: '+1 (555) 014-7314', gender: 'Female' },
  { id: 'PT-10485', name: 'Liam Anderson', age: 46, phone: '+1 (555) 014-1128', gender: 'Male' },
  { id: 'PT-10486', name: 'Emma Thompson', age: 41, phone: '+1 (555) 014-6632', gender: 'Female' },
  { id: 'PT-10487', name: 'Noah Davis', age: 23, phone: '+1 (555) 014-5207', gender: 'Male' },
];

export const doctors: Doctor[] = [
  { id: 'DR-201', name: 'Dr. Sarah Johnson', specialty: 'Cardiologist', department: 'Cardiology', room: 'Room 204', fee: 180, color: '#3f8f7b' },
  { id: 'DR-202', name: 'Dr. Michael Chen', specialty: 'Neurologist', department: 'Neurology', room: 'Room 118', fee: 220, color: '#4f7fc4' },
  { id: 'DR-203', name: 'Dr. Emily Davis', specialty: 'Dermatologist', department: 'Dermatology', room: 'Room 307', fee: 150, color: '#c38255' },
  { id: 'DR-204', name: 'Dr. Robert Williams', specialty: 'General Physician', department: 'Primary Care', room: 'Room 102', fee: 120, color: '#8272ae' },
];

export const initialAppointments: Appointment[] = [
  { id: 'APT-2026-001', patientId: 'PT-10482', doctorId: 'DR-201', date: '2026-10-24', time: '09:30', duration: 30, type: 'Consultation', reason: 'Routine heart check-up', notes: 'Bring previous ECG report.', status: 'Scheduled' },
  { id: 'APT-2026-002', patientId: 'PT-10483', doctorId: 'DR-202', date: '2026-10-24', time: '10:00', duration: 45, type: 'Follow-up', reason: 'Review MRI results', notes: '', status: 'Scheduled' },
  { id: 'APT-2026-003', patientId: 'PT-10484', doctorId: 'DR-203', date: '2026-10-24', time: '11:30', duration: 30, type: 'Consultation', reason: 'Skin irritation', notes: '', status: 'Completed' },
  { id: 'APT-2026-004', patientId: 'PT-10485', doctorId: 'DR-204', date: '2026-10-25', time: '09:00', duration: 30, type: 'Routine check-up', reason: 'Annual health screening', notes: '', status: 'Scheduled' },
  { id: 'APT-2026-005', patientId: 'PT-10486', doctorId: 'DR-201', date: '2026-10-25', time: '13:00', duration: 30, type: 'Follow-up', reason: 'Blood pressure follow-up', notes: 'Check medication list.', status: 'Scheduled' },
  { id: 'APT-2026-006', patientId: 'PT-10487', doctorId: 'DR-202', date: '2026-10-26', time: '14:30', duration: 45, type: 'Consultation', reason: 'Recurring headaches', notes: '', status: 'Scheduled' },
  { id: 'APT-2026-007', patientId: 'PT-10482', doctorId: 'DR-204', date: '2026-10-18', time: '15:00', duration: 30, type: 'Follow-up', reason: 'Discuss lab results', notes: '', status: 'Completed' },
  { id: 'APT-2026-008', patientId: 'PT-10483', doctorId: 'DR-203', date: '2026-10-20', time: '10:30', duration: 30, type: 'Consultation', reason: 'Mole assessment', notes: '', status: 'Cancelled' },
];
