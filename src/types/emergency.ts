export type EmergencyStatus = 'Waiting' | 'In Treatment' | 'Completed'

export interface EmergencyCase {
  id: number
  patientId: number
  patientName: string
  triageLevel: number // 1 = most critical, 5 = least urgent
  arrivalOrder: number
  status: EmergencyStatus
  notes: string
}

export interface EmergencyDraft {
  patientId: number
  triageLevel: number
  notes: string
}

export interface PatientOption {
  id: number
  name: string
}