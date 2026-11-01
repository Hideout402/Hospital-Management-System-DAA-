
import '../App.css';
import { useEffect, useMemo, useState } from 'react';
import { Search, Plus, Users, UserRound, Clock } from 'lucide-react';
import type { Patient } from '../mockdata/patients';

const API_URL = 'http://localhost:8080/api/patients';

const emptyForm = {
  name: '',
  age: '',
  gender: 'Female',
  phone: '',
  department: 'General Medicine',
};

export default function PatientPage() {
  const [patients, setPatients] = useState<Patient[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState('');

  const [search, setSearch] = useState('');
  const [statusFilter, setStatusFilter] = useState('All');
  const [showForm, setShowForm] = useState(false);
  const [editingId, setEditingId] = useState<number | null>(null);
  const [form, setForm] = useState({ ...emptyForm });

  // Selected patient for viewing visit history
  const [selectedPatient, setSelectedPatient] = useState<Patient | null>(null);
  const [historyLoading, setHistoryLoading] = useState(false);
  const [historyError, setHistoryError] = useState('');
  const [historyEntries, setHistoryEntries] = useState<string[]>([]);

  // Load patients from C++ backend
  async function loadPatients() {
    try {
      setLoading(true);
      setError('');

      const response = await fetch(API_URL);

      if (!response.ok) {
        throw new Error('Failed to load patients.');
      }

      const data: Patient[] = await response.json();
      setPatients(data);
    } catch {
      setError(
        'Cannot connect to the backend. Please make sure the C++ server is running.'
      );
    } finally {
      setLoading(false);
    }
  }

  // Load data when the page opens
  useEffect(() => {
    loadPatients();
  }, []);

  // Search and status filtering
  const filteredPatients = useMemo(() => {
    const query = search.trim().toLowerCase();

    return patients.filter((patient) => {
      const matchesSearch =
        patient.name.toLowerCase().includes(query) ||
        String(patient.id).toLowerCase().includes(query) ||
        patient.phone.includes(query);

      const matchesStatus =
        statusFilter === 'All' || patient.status === statusFilter;

      return matchesSearch && matchesStatus;
    });
  }, [patients, search, statusFilter]);

  const activeCount = patients.filter(
    (p) => p.status === 'Active'
  ).length;

  const dischargedCount = patients.filter(
    (p) => p.status === 'Discharged'
  ).length;

  async function openHistory(patient: Patient) {
    setSelectedPatient(patient);
    setHistoryLoading(true);
    setHistoryError('');
    setHistoryEntries([]);

    try {
      const response = await fetch(`${API_URL}/${patient.id}/history`);

      if (!response.ok) {
        const result = await response.json().catch(() => ({}));
        throw new Error(result.message || 'Failed to load visit history.');
      }

      const result: { visitHistory?: string[] } = await response.json();
      setHistoryEntries(Array.isArray(result.visitHistory) ? result.visitHistory : []);
    } catch (err) {
      setHistoryError(
        err instanceof Error ? err.message : 'Unable to load visit history.'
      );
    } finally {
      setHistoryLoading(false);
    }
  }

  function openAddForm() {
    setEditingId(null);
    setForm({ ...emptyForm });
    setShowForm(true);
  }

  function openEditForm(patient: Patient) {
    setEditingId(patient.id);

    setForm({
      name: patient.name,
      age: String(patient.age),
      gender: patient.gender,
      phone: patient.phone,
      department: patient.department,
    });

    setShowForm(true);
  }

  // Add or update patient through the backend
  async function handleSubmit(event: React.FormEvent<HTMLFormElement>) {
    event.preventDefault();

    const age = Number(form.age);

    if (
      !form.name.trim() ||
      !form.phone.trim() ||
      !Number.isInteger(age) ||
      age <= 0 ||
      age > 120
    ) {
      window.alert(
        'Enter a valid name, phone number, and age from 1 to 120.'
      );
      return;
    }

    try {
      const existingPatient = patients.find(
        (patient) => patient.id === editingId
      );

      const patientId =
        editingId !== null
          ? editingId
          : Math.max(1000, ...patients.map((patient) => patient.id)) + 1;

      const patientData = {
        id: patientId,
        name: form.name.trim(),
        age,
        gender: form.gender,
        phone: form.phone.trim(),
        address: existingPatient?.address ?? '',
        department: form.department,
        status: existingPatient?.status ?? 'Active',
        visitHistory: existingPatient?.visitHistory ?? [],
      };

      const response = await fetch(
        editingId !== null ? `${API_URL}/${patientId}` : API_URL,
        {
          method: editingId !== null ? 'PUT' : 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify(patientData),
        }
      );

      if (!response.ok) {
        const result = await response.json();
        throw new Error(result.message || 'Patient operation failed.');
      }

      await loadPatients();
      setShowForm(false);
    } catch (err) {
      window.alert(
        err instanceof Error ? err.message : 'Something went wrong.'
      );
    }
  }

  // Delete patient through the backend
  async function deletePatient(id: number) {
    if (
      !window.confirm('Are you sure you want to delete this patient?')
    ) {
      return;
    }

    try {
      const response = await fetch(`${API_URL}/${id}`, {
        method: 'DELETE',
      });

      if (!response.ok) {
        const result = await response.json();
        throw new Error(result.message || 'Could not delete patient.');
      }

      await loadPatients();
    } catch (err) {
      window.alert(
        err instanceof Error ? err.message : 'Something went wrong.'
      );
    }
  }

  return (
    <section className="patients-page">
      <div className="patients-heading">
        <div>
          <h2>Patients Management</h2>
          <p>View and manage patient records.</p>
        </div>

        <button
          className="patients-primary-button"
          onClick={openAddForm}
        >
          <Plus size={18} /> Add Patient
        </button>
      </div>

      <div className="patients-summary">
        <article className="patients-summary-card">
          <span className="patients-summary-icon">
            <Users size={21} />
          </span>
          <div>
            <p>Total Patients</p>
            <h3>{patients.length}</h3>
          </div>
        </article>

        <article className="patients-summary-card">
          <span className="patients-summary-icon">
            <UserRound size={21} />
          </span>
          <div>
            <p>Active Patients</p>
            <h3>{activeCount}</h3>
          </div>
        </article>

        <article className="patients-summary-card">
          <span className="patients-summary-icon">
            <Clock size={21} />
          </span>
          <div>
            <p>Discharged</p>
            <h3>{dischargedCount}</h3>
          </div>
        </article>
      </div>

      <div className="patients-table-card">
        <div className="patients-toolbar">
          <div className="patients-search">
            <Search size={18} />

            <input
              aria-label="Search patients"
              placeholder="Search by name, ID, or phone..."
              value={search}
              onChange={(event) => setSearch(event.target.value)}
            />
          </div>

          <select
            aria-label="Filter by patient status"
            value={statusFilter}
            onChange={(event) => setStatusFilter(event.target.value)}
          >
            <option value="All">All statuses</option>
            <option value="Active">Active</option>
            <option value="Discharged">Discharged</option>
          </select>
        </div>

        {loading && <p>Loading patients...</p>}

        {error && (
          <p role="alert">{error}</p>
        )}

        <div className="patients-table-scroll">
          <table className="patients-table">
            <thead>
              <tr>
                <th>Patient Name</th>
                <th>Patient ID</th>
                <th>Age / Gender</th>
                <th>Phone</th>
                <th>Department</th>
                <th>Status</th>
                <th>Actions</th>
              </tr>
            </thead>

            <tbody>
              {!loading && !error && filteredPatients.map((patient) => (
                <tr key={patient.id}>
                  <td>
                    <strong>{patient.name}</strong>
                  </td>

                  <td>{patient.id}</td>

                  <td>
                    {patient.age} / {patient.gender}
                  </td>

                  <td>{patient.phone}</td>

                  <td>{patient.department}</td>

                  <td>
                    <span
                      className={`patients-status ${patient.status.toLowerCase()}`}
                    >
                      {patient.status}
                    </span>
                  </td>

                  <td>
                    <div className="patients-actions">
                      <button
                        type="button"
                        onClick={() => openHistory(patient)}
                      >
                        View History
                      </button>

                      <button
                        type="button"
                        onClick={() => openEditForm(patient)}
                      >
                        Edit
                      </button>

                      <button
                        type="button"
                        className="patients-delete"
                        onClick={() => deletePatient(patient.id)}
                      >
                        Delete
                      </button>
                    </div>
                  </td>
                </tr>
              ))}

              {!loading && !error && filteredPatients.length === 0 && (
                <tr>
                  <td
                    colSpan={7}
                    className="patients-empty"
                  >
                    No patients found.
                  </td>
                </tr>
              )}
            </tbody>
          </table>
        </div>

        <div className="patients-table-footer">
          <span>
            Showing {filteredPatients.length} of {patients.length} patients
          </span>

          <span>Demo data · Changes are temporary</span>
        </div>
      </div>

      {/* Add / Edit Patient Modal */}
      {showForm && (
        <div
          className="patients-modal-backdrop"
          onClick={() => setShowForm(false)}
        >
          <section
            className="patients-modal"
            role="dialog"
            aria-modal="true"
            aria-labelledby="patient-form-title"
            onClick={(event) => event.stopPropagation()}
          >
            <div className="patients-modal-heading">
              <div>
                <h3 id="patient-form-title">
                  {editingId !== null ? 'Edit Patient' : 'Add Patient'}
                </h3>

                <p>Enter the patient details below.</p>
              </div>

              <button
                type="button"
                className="patients-close-button"
                aria-label="Close form"
                onClick={() => setShowForm(false)}
              >
                ×
              </button>
            </div>

            <form onSubmit={handleSubmit}>
              <label>
                Full Name

                <input
                  required
                  maxLength={100}
                  value={form.name}
                  onChange={(e) =>
                    setForm({ ...form, name: e.target.value })
                  }
                  placeholder="Enter full name"
                />
              </label>

              <div className="patients-form-row">
                <label>
                  Age

                  <input
                    required
                    type="number"
                    min="1"
                    max="120"
                    value={form.age}
                    onChange={(e) =>
                      setForm({ ...form, age: e.target.value })
                    }
                    placeholder="Age"
                  />
                </label>

                <label>
                  Gender

                  <select
                    value={form.gender}
                    onChange={(e) =>
                      setForm({ ...form, gender: e.target.value })
                    }
                  >
                    <option>Female</option>
                    <option>Male</option>
                    <option>Other</option>
                  </select>
                </label>
              </div>

              <label>
                Phone Number

                <input
                  required
                  maxLength={20}
                  value={form.phone}
                  onChange={(e) =>
                    setForm({ ...form, phone: e.target.value })
                  }
                  placeholder="Enter phone number"
                />
              </label>

              <label>
                Department

                <select
                  value={form.department}
                  onChange={(e) =>
                    setForm({ ...form, department: e.target.value })
                  }
                >
                  <option>General Medicine</option>
                  <option>Cardiology</option>
                  <option>Neurology</option>
                  <option>Orthopedics</option>
                  <option>Dermatology</option>
                  <option>Pediatrics</option>
                </select>
              </label>

              <div className="patients-form-actions">
                <button
                  type="button"
                  className="patients-cancel-button"
                  onClick={() => setShowForm(false)}
                >
                  Cancel
                </button>

                <button
                  type="submit"
                  className="patients-primary-button"
                >
                  {editingId !== null ? 'Save Changes' : 'Add Patient'}
                </button>
              </div>
            </form>
          </section>
        </div>
      )}

      {/* Patient Visit History Modal */}
      {selectedPatient && (
        <div
          className="patients-modal-backdrop"
          onClick={() => setSelectedPatient(null)}
        >
          <section
            className="patients-modal"
            role="dialog"
            aria-modal="true"
            aria-labelledby="visit-history-title"
            onClick={(event) => event.stopPropagation()}
          >
            <div className="patients-modal-heading">
              <div>
                <h3 id="visit-history-title">Patient Visit History</h3>
                <p>
                  {selectedPatient.name} · ID: {selectedPatient.id}
                </p>
              </div>

              <button
                type="button"
                className="patients-close-button"
                aria-label="Close history"
                onClick={() => setSelectedPatient(null)}
              >
                ×
              </button>
            </div>

            <h4>Previous Visits</h4>

            {historyLoading ? (
              <p>Loading visit history...</p>
            ) : historyError ? (
              <p role="alert">{historyError}</p>
            ) : historyEntries.length > 0 ? (
              <ol>
                {historyEntries.map((visit, index) => (
                  <li key={`${visit}-${index}`}>{visit}</li>
                ))}
              </ol>
            ) : (
              <p>No visit history available for this patient.</p>
            )}

            <button
              type="button"
              className="patients-primary-button"
              onClick={() => setSelectedPatient(null)}
            >
              Close
            </button>
          </section>
        </div>
      )}
    </section>
  );
}
