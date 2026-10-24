export type EmergencyStatus = 'Waiting' | 'In Treatment' | 'Done'

export interface EmergencyCase {
  id: string
  patientName: string
  condition: string
  priority: 1 | 2 | 3 | 4 | 5 // 1 = most critical
  arrivalTime: string // 'HH:MM'
  status: EmergencyStatus
}