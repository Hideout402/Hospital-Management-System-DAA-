import { apiRequest } from './apiRequest'
import type { HospitalLocation, ReachableResult, RouteResult } from '../types/navigation'

export const navigationApi = {
  getLocations: () => apiRequest<HospitalLocation[]>('/api/navigation/locations'),
  findRoute: (from: number, to: number) =>
    apiRequest<RouteResult>(`/api/navigation/route?from=${from}&to=${to}`),
  getReachable: (from: number) =>
    apiRequest<ReachableResult>(`/api/navigation/reachable?from=${from}`),
}