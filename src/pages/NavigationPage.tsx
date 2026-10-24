import '../App.css'
import { useState } from 'react'
import { Navigation } from 'lucide-react'
import { locations, corridors } from '../mockdata/navigation'
import type { Corridor } from '../types/navigation'

// BFS: finds the route with the fewest corridors. Time: O(V + E)
function findRoute(start: string, end: string, edges: Corridor[]): string[] {
  if (start === end) return [start]

  // Build adjacency list (corridors work in both directions)
  const adj = new Map<string, string[]>()
  edges.forEach(({ from, to }) => {
    adj.set(from, [...(adj.get(from) ?? []), to])
    adj.set(to, [...(adj.get(to) ?? []), from])
  })

  const visited = new Set<string>([start])
  const parent = new Map<string, string>()
  const queue: string[] = [start]

  while (queue.length > 0) {
    const current = queue.shift() as string
    for (const next of adj.get(current) ?? []) {
      if (!visited.has(next)) {
        visited.add(next)
        parent.set(next, current)
        if (next === end) {
          // Walk back through parents to build the path
          const path = [end]
          let node = end
          while (node !== start) {
            node = parent.get(node) as string
            path.unshift(node)
          }
          return path
        }
        queue.push(next)
      }
    }
  }
  return [] // no route found
}

export default function NavigationPage() {
  const [from, setFrom] = useState('entrance')
  const [to, setTo] = useState('icu')
  const [route, setRoute] = useState<string[]>([])
  const [searched, setSearched] = useState(false)

  const nameOf = (id: string) => locations.find((l) => l.id === id)?.name ?? id

  function handleFind() {
    setRoute(findRoute(from, to, corridors))
    setSearched(true)
  }

  // Which corridors are part of the route (for highlighting)
  const routeEdges = new Set<string>()
  for (let i = 0; i < route.length - 1; i++) {
    routeEdges.add(`${route[i]}|${route[i + 1]}`)
    routeEdges.add(`${route[i + 1]}|${route[i]}`)
  }

  return (
    <section className="nav-page">
      <h2 className="nav-title"><Navigation size={22} /> Hospital Navigation</h2>
      <p className="nav-sub">Choose two locations to find the shortest route.</p>

      <div className="nav-controls">
        <label>
          From
          <select value={from} onChange={(e) => setFrom(e.target.value)}>
            {locations.map((l) => (
              <option key={l.id} value={l.id}>{l.name}</option>
            ))}
          </select>
        </label>
        <label>
          To
          <select value={to} onChange={(e) => setTo(e.target.value)}>
            {locations.map((l) => (
              <option key={l.id} value={l.id}>{l.name}</option>
            ))}
          </select>
        </label>
        <button className="nav-btn" onClick={handleFind}>Find route</button>
      </div>

      {searched && (
        <div className="nav-result">
          {route.length === 0 ? (
            <span>No route found between these locations.</span>
          ) : (
            <span>
              <strong>Route ({route.length - 1} corridor{route.length - 1 === 1 ? '' : 's'}):</strong>{' '}
              {route.map(nameOf).join('  →  ')}
            </span>
          )}
        </div>
      )}

      <svg className="nav-map" viewBox="0 0 600 360">
        {corridors.map((c) => {
          const a = locations.find((l) => l.id === c.from)
          const b = locations.find((l) => l.id === c.to)
          if (!a || !b) return null
          const active = routeEdges.has(`${c.from}|${c.to}`)
          return (
            <line
              key={`${c.from}-${c.to}`}
              x1={a.x} y1={a.y} x2={b.x} y2={b.y}
              className={active ? 'nav-edge nav-edge-active' : 'nav-edge'}
            />
          )
        })}
        {locations.map((l) => {
          const active = route.includes(l.id)
          return (
            <g key={l.id}>
              <circle
                cx={l.x} cy={l.y} r={18}
                className={active ? 'nav-node nav-node-active' : 'nav-node'}
              />
              <text x={l.x} y={l.y + 34} textAnchor="middle" className="nav-label">
                {l.name}
              </text>
            </g>
          )
        })}
      </svg>
    </section>
  )
}