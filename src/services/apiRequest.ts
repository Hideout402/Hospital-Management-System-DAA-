export const API_BASE = 'http://localhost:8080'

export async function apiRequest<T>(path: string, init?: RequestInit): Promise<T> {
  let response: Response
  try {
    response = await fetch(`${API_BASE}${path}`, {
      ...init,
      headers: {
        'Content-Type': 'application/json',
        ...init?.headers,
      },
    })
  } catch {
    throw new Error('Cannot connect to the backend. Check that the C++ server is running.')
  }

  const body = await response.text()
  let payload: unknown
  if (body) {
    try {
      payload = JSON.parse(body) as unknown
    } catch {
      if (response.ok) throw new Error('The backend returned an invalid response.')
    }
  } else if (response.ok) {
    throw new Error('The backend returned an invalid response.')
  }

  if (!response.ok) {
    const message =
      typeof payload === 'object' && payload !== null &&
      'message' in payload && typeof payload.message === 'string'
        ? payload.message
        : `Request failed (${response.status}).`
    throw new Error(message)
  }

  return payload as T
}