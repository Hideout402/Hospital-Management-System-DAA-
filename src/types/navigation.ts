export interface HospitalLocation {
  id: number
  name: string
  connectedLocationIds: number[]
}

export interface RouteResult {
  from: number
  to: number
  corridors: number
  path: number[]
  pathNames: string[]
}

export interface ReachableResult {
  from: number
  count: number
  order: number[]
  orderNames: string[]
}