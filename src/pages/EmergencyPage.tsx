import '../App.css'
import { useMemo, useState, type FormEvent } from 'react'
import { Siren, Plus } from 'lucide-react'
import { initialEmergencyCases } from '../mockdata/emergency'
import type { EmergencyCase } from '../types/emergency'

const priorityLabels: Record<number, string> = {
  1: 'Critical',
  2: 'Emergent',
  3: 'Urgent',
  4: 'Less urgent',
  5: 'Non-urgent',
}

const emptyForm = { patientName: '', condition: '', priority: '3' }

export default function EmergencyPage() {
  const [cases, setCases] = useState<EmergencyCase[]>(initialEmergencyCases)
  const [showForm, setShowForm] = useState(false)
  const [form, setForm] = useState({ ...emptyForm })
  const [error, setError] = useState('')

  // Sort: finished cases last, then by priority, then by arrival time
  const sortedCases = useMemo(() => {
    return [...cases].sort((a, b) => {
      const aDone = a.status === 'Done' ? 1 : 0
      const bDone = b.status === 'Done' ? 1 : 0
      return (
        aDone - bDone ||
        a.priority - b.priority ||
        a.arrivalTime.localeCompare(b.arrivalTime)
      )
    })
  }, [cases])

  const waitingCount = cases.filter((c) => c.status === 'Waiting').length
  const nextCase = sortedCases.find((c) => c.status === 'Waiting')

  function handleAdd(e: FormEvent) {
    e.preventDefault()
    if (!form.patientName.trim() || !form.condition.trim()) {
      setError('Patient name and condition are required.')
      return
    }
    const newCase: EmergencyCase = {
      id: `EM-${Date.now().toString().slice(-5)}`,
      patientName: form.patientName.trim(),
      condition: form.condition.trim(),
      priority: Number(form.priority) as EmergencyCase['priority'],
      arrivalTime: new Date().toTimeString().slice(0, 5),
      status: 'Waiting',
    }
    setCases([...cases, newCase])
    setForm({ ...emptyForm })
    setError('')
    setShowForm(false)
  }

  function treatNext() {
    if (!nextCase) return
    setCases(cases.map((c) => (c.id === nextCase.id ? { ...c, status: 'In Treatment' } : c)))
  }

  function markDone(id: string) {
    setCases(cases.map((c) => (c.id === id ? { ...c, status: 'Done' } : c)))
  }

  return (
    <section className="em-page">
      <div className="em-top">
        <div>
          <h2 className="em-title"><Siren size={22} /> Emergency Queue</h2>
          <p className="em-sub">
            {waitingCount} waiting. Most critical patient is shown first.
          </p>
        </div>
        <div className="em-actions">
          <button className="em-btn em-btn-light" onClick={treatNext} disabled={!nextCase}>
            Treat next patient
          </button>
          <button className="em-btn" onClick={() => setShowForm(!showForm)}>
            <Plus size={16} /> Add case
          </button>
        </div>
      </div>

      {showForm && (
        <form className="em-form" onSubmit={handleAdd}>
          <input
            placeholder="Patient name"
            value={form.patientName}
            onChange={(e) => setForm({ ...form, patientName: e.target.value })}
          />
          <input
            placeholder="Condition"
            value={form.condition}
            onChange={(e) => setForm({ ...form, condition: e.target.value })}
          />
          <select
            value={form.priority}
            onChange={(e) => setForm({ ...form, priority: e.target.value })}
          >
            {[1, 2, 3, 4, 5].map((p) => (
              <option key={p} value={p}>{p} - {priorityLabels[p]}</option>
            ))}
          </select>
          <button className="em-btn" type="submit">Save</button>
          {error && <p className="em-error">{error}</p>}
        </form>
      )}

      {sortedCases.length === 0 ? (
        <p className="em-empty">No emergency cases right now.</p>
      ) : (
        <table className="em-table">
          <thead>
            <tr>
              <th>ID</th><th>Patient</th><th>Condition</th>
              <th>Priority</th><th>Arrival</th><th>Status</th><th></th>
            </tr>
          </thead>
          <tbody>
            {sortedCases.map((c) => (
              <tr key={c.id}>
                <td>{c.id}</td>
                <td>{c.patientName}</td>
                <td>{c.condition}</td>
                <td>
                  <span className={`em-badge em-p${c.priority}`}>
                    {c.priority} - {priorityLabels[c.priority]}
                  </span>
                </td>
                <td>{c.arrivalTime}</td>
                <td>{c.status}</td>
                <td>
                  {c.status === 'In Treatment' && (
                    <button className="em-btn em-btn-small" onClick={() => markDone(c.id)}>
                      Mark done
                    </button>
                  )}
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      )}
    </section>
  )
}