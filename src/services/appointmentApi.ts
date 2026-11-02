import type {
  Appointment,
  AppointmentDraft,
  AppointmentAction,
  AppointmentOptions,
  AppointmentQueueState,
  AppointmentSearchResult,
  AppointmentSortAlgorithm,
  AppointmentSortField,
  AppointmentSortResult,
} from '@/types/appointments';

const API_URL = 'http://localhost:8080/api/appointments';

async function request<T>(url: string, init?: RequestInit): Promise<T> {
  let response: Response;
  try {
    response = await fetch(url, {
      ...init,
      headers: {
        'Content-Type': 'application/json',
        ...init?.headers,
      },
    });
  } catch {
    throw new Error('Cannot connect to the appointment backend. Check that the C++ server is running.');
  }

  const responseBody = await response.text();
  let payload: unknown;
  if (responseBody) {
    try {
      payload = JSON.parse(responseBody) as unknown;
    } catch {
      if (response.ok)
        throw new Error('The appointment backend returned an invalid response.');
    }
  } else if (response.ok) {
    throw new Error('The appointment backend returned an invalid response.');
  }

  if (!response.ok) {
    const message = typeof payload === 'object' && payload !== null &&
      'message' in payload && typeof payload.message === 'string'
      ? payload.message
      : `Appointment request failed (${response.status}).`;
    throw new Error(message);
  }

  return payload as T;
}

interface AppointmentMutationResponse {
  appointment: Appointment;
}

export const appointmentApi = {
  getAll: () => request<Appointment[]>(API_URL),
  getById: (id: number) => request<Appointment>(`${API_URL}/${id}`),
  getOptions: () => request<AppointmentOptions>(`${API_URL}/options`),
  create: (draft: AppointmentDraft) => request<AppointmentMutationResponse>(API_URL, {
    method: 'POST',
    body: JSON.stringify(draft),
  }),
  update: (id: number, draft: AppointmentDraft) =>
    request<AppointmentMutationResponse>(`${API_URL}/${id}`, {
      method: 'PUT',
      body: JSON.stringify(draft),
    }),
  cancel: (id: number) =>
    request<AppointmentMutationResponse>(`${API_URL}/${id}/cancel`, {
      method: 'PATCH',
    }),
  sort: (algorithm: AppointmentSortAlgorithm, by: AppointmentSortField) =>
    request<AppointmentSortResult>(
      `${API_URL}/dsa/sort?algorithm=${algorithm}&by=${by}`,
    ),
  searchById: (id: number) =>
    request<AppointmentSearchResult>(`${API_URL}/dsa/search?id=${id}`),
  getCheckInQueue: () =>
    request<AppointmentQueueState>(`${API_URL}/dsa/queue`),
  enqueueCheckIn: (appointmentId: number) =>
    request<{ success: boolean; message: string }>(`${API_URL}/dsa/queue`, {
      method: 'POST',
      body: JSON.stringify({ appointmentId }),
    }),
  dequeueCheckIn: () =>
    request<{ success: boolean; message: string; appointmentId: number }>(
      `${API_URL}/dsa/queue`,
      { method: 'DELETE' },
    ),
  getWaitingRoomQueue: () =>
    request<AppointmentQueueState>(`${API_URL}/dsa/circular-queue`),
  enqueueWaitingRoom: (appointmentId: number) =>
    request<{ success: boolean; message: string }>(
      `${API_URL}/dsa/circular-queue`,
      { method: 'POST', body: JSON.stringify({ appointmentId }) },
    ),
  dequeueWaitingRoom: () =>
    request<{ success: boolean; message: string; appointmentId: number }>(
      `${API_URL}/dsa/circular-queue`,
      { method: 'DELETE' },
    ),
  rotateWaitingRoom: () =>
    request<{ success: boolean; message: string; appointmentId: number }>(
      `${API_URL}/dsa/circular-queue/rotate`,
      { method: 'POST' },
    ),
  getActionHistory: () =>
    request<AppointmentAction[]>(`${API_URL}/dsa/history`),
  peekActionHistory: () =>
    request<AppointmentAction>(`${API_URL}/dsa/history/peek`),
  popActionHistory: () =>
    request<{ success: boolean; message: string; action: AppointmentAction }>(
      `${API_URL}/dsa/history`,
      { method: 'DELETE' },
    ),
};
