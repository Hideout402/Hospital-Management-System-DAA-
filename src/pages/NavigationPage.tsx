import '../App.css'
import { useEffect, useState } from 'react'
import { Navigation } from 'lucide-react'
import { navigationApi } from '../services/navigationApi'
import { locationPositions } from '../mockdata/navigation'
import type { HospitalLocation, RouteResult } from '../types/navigation'

function edgeKey(a: number, b: number) {
  return a < b ? `${a}-${b}` : `${b}-${a}`
}

function getPosition(id: number, index: number, total: number) {
  const fixed = locationPositions[id]
  if (fixed) return fixed
  // Fallback for locations without a saved position: place on a circle
  const angle = (index / total) * 2 * Math.PI
  return { x: 300 + 220 * Math.cos(angle), y: 180 + 140 * Math.sin(angle) }
}

export default function NavigationPage() {
  const [locations, setLocations] = useState<HospitalLocation[]>([])
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState('')
  const [from, setFrom] = useState(0)
  const [to, setTo] = useState(0)
  const [route, setRoute] = useState<RouteResult | null>(null)
  const [routeError, setRouteError] = useState('')
  const [reachable, setReachable] = useState<string[] | null>(null)
  const [busy, setBusy] = useState(false)

  async function loadLocations() {
    setLoading(true)
    try {
      const data = await navigationApi.getLocations()
      setLocations(data)
      if (data.length > 0) {
        setFrom(data[0].id)
        setTo(data[data.length - 1].id)
      }
      setError('')
    } catch (e) {
      setError(e instanceof Error ? e.message : 'Failed to load locations.')
    } finally {
      setLoading(false)
    }
  }

  useEffect(() => {
    loadLocations()
  }, [])

  async function handleFindRoute() {
    setBusy(true)
    setReachable(null)
    try {
      setRoute(await navigationApi.findRoute(from, to))
      setRouteError('')
    } catch (e) {
      setRoute(null)
      setRouteError(e instanceof Error ? e.message : 'Could not find a route.')
    } finally {
      setBusy(false)
    }
  }

  async function handleReachable() {
    setBusy(true)
    setRoute(null)
    setRouteError('')
    try {
      const result = await navigationApi.getReachable(from)
      setReachable(result.orderNames)
    } catch (e) {
      setReachable(null)
      setRouteError(e instanceof Error ? e.message : 'Could not load reachable locations.')
    } finally {
      setBusy(false)
    }
  }

  // Corridors, each drawn once
  const corridorKeys = new Set<string>()
  locations.forEach((l) =>
    l.connectedLocationIds.forEach((next) => corridorKeys.add(edgeKey(l.id, next))),
  )

  // Corridors that belong to the route
  const routeKeys = new Set<string>()
  if (route) {
    for (let i = 0; i < route.path.length - 1; i++) {
      routeKeys.add(edgeKey(route.path[i], route.path[i + 1]))
    }
  }

  const positionOf = (id: number) => {
    const index = locations.findIndex((l) => l.id === id)
    return getPosition(id, index, locations.length)
  }

  return (
    <section className="nav-page">
      <h2 className="nav-title"><Navigation size={22} /> Hospital Navigation</h2>
      <p className="nav-sub">Choose two locations to find the shortest route.</p>

      {error && (
        <div className="em-banner">
          <span>{error}</span>
          <button className="em-btn em-btn-small" onClick={loadLocations}>Retry</button>
        </div>
      )}

      {loading ? (
        <p className="em-loading">Loading hospital map...</p>
      ) : locations.length === 0 ? (
        <p className="em-empty">No hospital locations available.</p>
      ) : (
        <>
          <div className="nav-controls">
            <label>
              From
              <select value={from} onChange={(e) => setFrom(Number(e.target.value))}>
                {locations.map((l) => (
                  <option key={l.id} value={l.id}>{l.name}</option>
                ))}
              </select>
            </label>
            <label>
              To
              <select value={to} onChange={(e) => setTo(Number(e.target.value))}>
                {locations.map((l) => (
                  <option key={l.id} value={l.id}>{l.name}</option>
                ))}
              </select>
            </label>
            <button className="nav-btn" disabled={busy} onClick={handleFindRoute}>
              Find route
            </button>
            <button className="nav-btn nav-btn-light" disabled={busy} onClick={handleReachable}>
              Show reachable (DFS)
            </button>
          </div>

          {routeError && <div className="em-banner"><span>{routeError}</span></div>}

          {route && (
            <div className="nav-result">
              <strong>
                Route ({route.corridors} corridor{route.corridors === 1 ? '' : 's'}):
              </strong>{' '}
              {route.pathNames.join('  →  ')}
            </div>
          )}

          {reachable && (
            <div className="nav-result">
              <strong>Reachable from {locations.find((l) => l.id === from)?.name} (DFS order):</strong>{' '}
              {reachable.join('  →  ')}
            </div>
          )}

          <svg className="nav-map" viewBox="0 0 600 360">
            {[...corridorKeys].map((key) => {
              const [a, b] = key.split('-').map(Number)
              const pa = positionOf(a)
              const pb = positionOf(b)
              return (
                <line
                  key={key}
                  x1={pa.x} y1={pa.y} x2={pb.x} y2={pb.y}
                  className={routeKeys.has(key) ? 'nav-edge nav-edge-active' : 'nav-edge'}
                />
              )
            })}
            {locations.map((l) => {
              const p = positionOf(l.id)
              const active = route ? route.path.includes(l.id) : false
              return (
                <g key={l.id}>
                  <circle
                    cx={p.x} cy={p.y} r={18}
                    className={active ? 'nav-node nav-node-active' : 'nav-node'}
                  />
                  <text x={p.x} y={p.y + 34} textAnchor="middle" className="nav-label">
                    {l.name}
                  </text>
                </g>
              )
            })}
          </svg>
        </>
      )}
    </section>
  )
}