import { apiRequest } from './apiRequest'
import type {
  EmergencyCase,
  EmergencyDraft,
  EmergencyStatus,
  PatientOption,
} from '../types/emergency'

export const emergencyApi = {
  getAll: () => apiRequest<EmergencyCase[]>('/api/emergency'),
  create: (draft: EmergencyDraft) =>
    apiRequest<EmergencyCase>('/api/emergency', {
      method: 'POST',
      body: JSON.stringify(draft),
    }),
  treatNext: () =>
    apiRequest<EmergencyCase>('/api/emergency/treat-next', { method: 'POST' }),
  updateStatus: (id: number, status: EmergencyStatus) =>
    apiRequest<EmergencyCase>(`/api/emergency/${id}/status`, {
      method: 'PATCH',
      body: JSON.stringify({ status }),
    }),
  // M1's existing patients endpoint, used for the patient dropdown
  getPatients: () => apiRequest<PatientOption[]>('/api/patients'),
}