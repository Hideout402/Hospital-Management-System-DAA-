import type { EmergencyCase } from '../types/emergency'

export const initialEmergencyCases: EmergencyCase[] = [
  { id: 'EM-001', patientName: 'Ali Raza', condition: 'Chest pain', priority: 1, arrivalTime: '09:10', status: 'Waiting' },
  { id: 'EM-002', patientName: 'Sara Khan', condition: 'Broken arm', priority: 3, arrivalTime: '09:00', status: 'Waiting' },
  { id: 'EM-003', patientName: 'Hamza Ahmed', condition: 'High fever', priority: 4, arrivalTime: '08:45', status: 'Waiting' },
  { id: 'EM-004', patientName: 'Fatima Noor', condition: 'Severe bleeding', priority: 2, arrivalTime: '09:20', status: 'In Treatment' },
  { id: 'EM-005', patientName: 'Usman Tariq', condition: 'Small cut', priority: 5, arrivalTime: '08:30', status: 'Waiting' },
]