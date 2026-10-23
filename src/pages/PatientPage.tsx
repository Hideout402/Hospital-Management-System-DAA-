import '../App.css';
import { useMemo, useState } from 'react';
import { Search, Plus, Users, UserRound, Clock } from 'lucide-react';
import { initialPatients, type Patient } from '../mockdata/patients';

const emptyForm = {
  name: '',
  age: '',
  gender: 'Female',
  phone: '',
  department: 'General Medicine',
};

export default function PatientPage() {
  const [patients, setPatients] = useState<Patient[]>(initialPatients);
  const [search, setSearch] = useState('');
  const [statusFilter, setStatusFilter] = useState('All');
  const [showForm, setShowForm] = useState(false);
  const [editingId, setEditingId] = useState<string | null>(null);
  const [form, setForm] = useState({ ...emptyForm });

  const filteredPatients = useMemo(() => {
    const query = search.trim().toLowerCase();

    return patients.filter((patient) => {
      const matchesSearch =
        patient.name.toLowerCase().includes(query) ||
        patient.id.toLowerCase().includes(query) ||
        patient.phone.includes(query);

      const matchesStatus =
        statusFilter === 'All' || patient.status === statusFilter;

      return matchesSearch && matchesStatus;
    });
  }, [patients, search, statusFilter]);

  const activeCount = patients.filter((p) => p.status === 'Active').length;
  const dischargedCount = patients.filter((p) => p.status === 'Discharged').length;

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

  function handleSubmit(event: React.FormEvent<HTMLFormElement>) {
    event.preventDefault();

    const age = Number(form.age);

    if (
      !form.name.trim() ||
      !form.phone.trim() ||
      !Number.isInteger(age) ||
      age < 0 ||
      age > 120
    ) {
      window.alert('Enter a valid name, phone number, and age from 0 to 120.');
      return;
    }

    if (editingId) {
      setPatients((current) =>
        current.map((patient) =>
          patient.id === editingId
            ? { ...patient, ...form, name: form.name.trim(), age }
            : patient
        )
      );
    } else {
      const newPatient: Patient = {
        ...form,
        name: form.name.trim(),
        age,
        id: `PAT-${Date.now().toString().slice(-6)}`,
        status: 'Active',
      };

      setPatients((current) => [newPatient, ...current]);
    }

    setShowForm(false);
  }

  function deletePatient(id: string) {
    if (window.confirm('Are you sure you want to delete this patient?')) {
      setPatients((current) => current.filter((patient) => patient.id !== id));
    }
  }

  return (
    <section className="patients-page">
      <div className="patients-heading">
        <div>
          <h2>Patients Management</h2>
          <p>View and manage patient records.</p>
        </div>
        <button className="patients-primary-button" onClick={openAddForm}>
          <Plus size={18} /> Add Patient
        </button>
      </div>

      <div className="patients-summary">
        <article className="patients-summary-card">
          <span className="patients-summary-icon"><Users size={21} /></span>
          <div><p>Total Patients</p><h3>{patients.length}</h3></div>
        </article>
        <article className="patients-summary-card">
          <span className="patients-summary-icon"><UserRound size={21} /></span>
          <div><p>Active Patients</p><h3>{activeCount}</h3></div>
        </article>
        <article className="patients-summary-card">
          <span className="patients-summary-icon"><Clock size={21} /></span>
          <div><p>Discharged</p><h3>{dischargedCount}</h3></div>
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
              {filteredPatients.map((patient) => (
                <tr key={patient.id}>
                  <td><strong>{patient.name}</strong></td>
                  <td>{patient.id}</td>
                  <td>{patient.age} / {patient.gender}</td>
                  <td>{patient.phone}</td>
                  <td>{patient.department}</td>
                  <td>
                    <span className={`patients-status ${patient.status.toLowerCase()}`}>
                      {patient.status}
                    </span>
                  </td>
                  <td>
                    <div className="patients-actions">
                      <button onClick={() => openEditForm(patient)}>Edit</button>
                      <button
                        className="patients-delete"
                        onClick={() => deletePatient(patient.id)}
                      >
                        Delete
                      </button>
                    </div>
                  </td>
                </tr>
              ))}
              {filteredPatients.length === 0 && (
                <tr>
                  <td colSpan={7} className="patients-empty">No patients found.</td>
                </tr>
              )}
            </tbody>
          </table>
        </div>

        <div className="patients-table-footer">
          <span>Showing {filteredPatients.length} of {patients.length} patients</span>
          <span>Demo data · Changes are temporary</span>
        </div>
      </div>

      {showForm && (
        <div className="patients-modal-backdrop" onClick={() => setShowForm(false)}>
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
                  {editingId ? 'Edit Patient' : 'Add Patient'}
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
                  onChange={(e) => setForm({ ...form, name: e.target.value })}
                  placeholder="Enter full name"
                />
              </label>

              <div className="patients-form-row">
                <label>
                  Age
                  <input
                    required
                    type="number"
                    min="0"
                    max="120"
                    value={form.age}
                    onChange={(e) => setForm({ ...form, age: e.target.value })}
                    placeholder="Age"
                  />
                </label>
                <label>
                  Gender
                  <select
                    value={form.gender}
                    onChange={(e) => setForm({ ...form, gender: e.target.value })}
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
                  onChange={(e) => setForm({ ...form, phone: e.target.value })}
                  placeholder="Enter phone number"
                />
              </label>

              <label>
                Department
                <select
                  value={form.department}
                  onChange={(e) => setForm({ ...form, department: e.target.value })}
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
                <button type="submit" className="patients-primary-button">
                  {editingId ? 'Save Changes' : 'Add Patient'}
                </button>
              </div>
            </form>
          </section>
        </div>
      )}
    </section>
  );
}
