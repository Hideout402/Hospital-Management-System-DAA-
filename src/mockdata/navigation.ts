import type { LocationNode, Corridor } from '../types/navigation'

export const locations: LocationNode[] = [
  { id: 'entrance', name: 'Entrance', x: 60, y: 180 },
  { id: 'reception', name: 'Reception', x: 170, y: 180 },
  { id: 'er', name: 'Emergency Room', x: 290, y: 70 },
  { id: 'lab', name: 'Laboratory', x: 290, y: 180 },
  { id: 'pharmacy', name: 'Pharmacy', x: 290, y: 290 },
  { id: 'ward', name: 'Ward A', x: 440, y: 70 },
  { id: 'radiology', name: 'Radiology', x: 440, y: 180 },
  { id: 'icu', name: 'ICU', x: 440, y: 290 },
]

export const corridors: Corridor[] = [
  { from: 'entrance', to: 'reception' },
  { from: 'reception', to: 'er' },
  { from: 'reception', to: 'lab' },
  { from: 'reception', to: 'pharmacy' },
  { from: 'er', to: 'lab' },
  { from: 'er', to: 'ward' },
  { from: 'lab', to: 'radiology' },
  { from: 'ward', to: 'radiology' },
  { from: 'pharmacy', to: 'icu' },
  { from: 'radiology', to: 'icu' },
]