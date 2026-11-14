import '../App.css'
import { useCallback, useEffect, useState, type FormEvent } from 'react'
import { Siren, Plus } from 'lucide-react'
import { emergencyApi } from '../services/emergencyApi'
import type { EmergencyCase, PatientOption } from '../types/emergency'

const triageLabels: Record<number, string> = {
  1: 'Critical',
  2: 'Emergent',
  3: 'Urgent',
  4: 'Less urgent',
  5: 'Non-urgent',
}

const emptyForm = { patientId: '', triageLevel: '3', notes: '' }

export default function EmergencyPage() {
  const [cases, setCases] = useState<EmergencyCase[]>([])
  const [patients, setPatients] = useState<PatientOption[]>([])
  const [loading, setLoading] = useState(true)
  const [busy, setBusy] = useState(false)
  const [error, setError] = useState('')
  const [showForm, setShowForm] = useState(false)
  const [form, setForm] = useState({ ...emptyForm })
  const [formError, setFormError] = useState('')

  const loadCases = useCallback(async () => {
    try {
      setCases(await emergencyApi.getAll())
      setError('')
    } catch (e) {
      setError(e instanceof Error ? e.message : 'Failed to load emergency cases.')
    } finally {
      setLoading(false)
    }
  }, [])

  useEffect(() => {
    loadCases()
    emergencyApi.getPatients().then(setPatients).catch(() => setPatients([]))
  }, [loadCases])

  async function runAction(action: () => Promise<unknown>) {
    setBusy(true)
    try {
      await action()
      await loadCases()
    } catch (e) {
      setError(e instanceof Error ? e.message : 'Request failed.')
    } finally {
      setBusy(false)
    }
  }

  async function handleAdd(e: FormEvent) {
    e.preventDefault()
    if (!form.patientId) {
      setFormError('Please choose a patient.')
      return
    }
    if (!form.notes.trim()) {
      setFormError('Please describe the condition.')
      return
    }

    setBusy(true)
    try {
      await emergencyApi.create({
        patientId: Number(form.patientId),
        triageLevel: Number(form.triageLevel),
        notes: form.notes.trim(),
      })
      await loadCases()
      setForm({ ...emptyForm })
      setFormError('')
      setShowForm(false)
    } catch (err) {
      setFormError(err instanceof Error ? err.message : 'Could not add the case.')
    } finally {
      setBusy(false)
    }
  }

  const waitingCount = cases.filter((c) => c.status === 'Waiting').length

  return (
    <section className="em-page">
      <div className="em-top">
        <div>
          <h2 className="em-title"><Siren size={22} /> Emergency Queue</h2>
          <p className="em-sub">
            {waitingCount} waiting. The most critical patient is shown first.
          </p>
        </div>
        <div className="em-actions">
          <button
            className="em-btn em-btn-light"
            disabled={busy || waitingCount === 0}
            onClick={() => runAction(() => emergencyApi.treatNext())}
          >
            Treat next patient
          </button>
          <button className="em-btn" onClick={() => setShowForm(!showForm)}>
            <Plus size={16} /> Add case
          </button>
        </div>
      </div>

      {error && (
        <div className="em-banner">
          <span>{error}</span>
          <button className="em-btn em-btn-small" onClick={loadCases}>Retry</button>
        </div>
      )}

      {showForm && (
        <form className="em-form" onSubmit={handleAdd}>
          <select
            value={form.patientId}
            onChange={(e) => setForm({ ...form, patientId: e.target.value })}
          >
            <option value="">Select patient</option>
            {patients.map((p) => (
              <option key={p.id} value={p.id}>{p.id} - {p.name}</option>
            ))}
          </select>
          <select
            value={form.triageLevel}
            onChange={(e) => setForm({ ...form, triageLevel: e.target.value })}
          >
            {[1, 2, 3, 4, 5].map((level) => (
              <option key={level} value={level}>{level} - {triageLabels[level]}</option>
            ))}
          </select>
          <input
            placeholder="Condition / notes"
            value={form.notes}
            onChange={(e) => setForm({ ...form, notes: e.target.value })}
          />
          <button className="em-btn" type="submit" disabled={busy}>Save</button>
          {formError && <p className="em-error">{formError}</p>}
        </form>
      )}

      {loading ? (
        <p className="em-loading">Loading emergency cases...</p>
      ) : cases.length === 0 ? (
        <p className="em-empty">No emergency cases right now.</p>
      ) : (
        <table className="em-table">
          <thead>
            <tr>
              <th>#</th><th>Patient</th><th>Condition</th>
              <th>Triage</th><th>Status</th><th></th>
            </tr>
          </thead>
          <tbody>
            {cases.map((c) => (
              <tr key={c.id}>
                <td>{c.arrivalOrder}</td>
                <td>{c.patientName}</td>
                <td>{c.notes}</td>
                <td>
                  <span className={`em-badge em-p${c.triageLevel}`}>
                    {c.triageLevel} - {triageLabels[c.triageLevel] ?? 'Unknown'}
                  </span>
                </td>
                <td>{c.status}</td>
                <td>
                  {c.status === 'In Treatment' && (
                    <button
                      className="em-btn em-btn-small"
                      disabled={busy}
                      onClick={() =>
                        runAction(() => emergencyApi.updateStatus(c.id, 'Completed'))
                      }
                    >
                      Mark completed
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