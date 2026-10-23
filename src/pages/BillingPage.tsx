
import { useMemo, useState } from 'react';
import { Search, ReceiptText, CheckCircle, Clock, Plus, X } from 'lucide-react';
import { initialBills, type Bill, type BillStatus } from '../mockdata/billing';
import '../App.css';

type BillForm = {
  patientId: string;
  patientName: string;
  date: string;
  amount: string;
  status: BillStatus;
};

const emptyForm: BillForm = {
  patientId: '',
  patientName: '',
  date: new Date().toISOString().slice(0, 10),
  amount: '',
  status: 'Pending',
};

export default function BillingPage() {
  const [bills, setBills] = useState<Bill[]>(initialBills);
  const [search, setSearch] = useState('');
  const [filter, setFilter] = useState<'All' | BillStatus>('All');
  const [showForm, setShowForm] = useState(false);
  const [editingId, setEditingId] = useState<string | null>(null);
  const [form, setForm] = useState<BillForm>(emptyForm);
  const [formError, setFormError] = useState('');

  const filteredBills = useMemo(() => {
    const query = search.trim().toLowerCase();

    return bills.filter((bill) => {
      const matchesSearch =
        bill.patientId.toLowerCase().includes(query) ||
        bill.patientName.toLowerCase().includes(query) ||
        bill.id.toLowerCase().includes(query);

      return matchesSearch && (filter === 'All' || bill.status === filter);
    });
  }, [bills, search, filter]);

  const paidBills = bills.filter((bill) => bill.status === 'Paid');
  const pendingBills = bills.filter((bill) => bill.status === 'Pending');
  const paidAmount = paidBills.reduce((sum, bill) => sum + bill.amount, 0);
  const pendingAmount = pendingBills.reduce((sum, bill) => sum + bill.amount, 0);

  const formatAmount = (amount: number) =>
    `Rs. ${amount.toLocaleString('en-PK')}`;

  function openAddForm() {
    setEditingId(null);
    setForm({ ...emptyForm, date: new Date().toISOString().slice(0, 10) });
    setFormError('');
    setShowForm(true);
  }

  function openEditForm(bill: Bill) {
    setEditingId(bill.id);
    setForm({
      patientId: bill.patientId,
      patientName: bill.patientName,
      date: bill.date,
      amount: String(bill.amount),
      status: bill.status,
    });
    setFormError('');
    setShowForm(true);
  }

  function handleSubmit(event: React.FormEvent<HTMLFormElement>) {
    event.preventDefault();

    const amount = Number(form.amount);
    if (!Number.isFinite(amount) || amount <= 0) {
      setFormError('Please enter an amount greater than zero.');
      return;
    }

    const patientId = form.patientId.trim().toUpperCase();
    const patientName = form.patientName.trim();

    if (!patientId || !patientName || !form.date) {
      setFormError('Please complete all fields.');
      return;
    }

    if (editingId) {
      setBills((current) =>
        current.map((bill) =>
          bill.id === editingId
            ? {
                ...bill,
                patientId,
                patientName,
                date: form.date,
                amount,
                status: form.status,
              }
            : bill,
        ),
      );
    } else {
      const newBill: Bill = {
        id: `BILL-${Date.now().toString().slice(-6)}`,
        patientId,
        patientName,
        date: form.date,
        amount,
        status: form.status,
      };

      setBills((current) => [newBill, ...current]);
    }

    setShowForm(false);
    setEditingId(null);
    setForm({ ...emptyForm });
    setFormError('');
  }

  function deleteBill(billId: string) {
    if (!window.confirm(`Delete bill ${billId}? This cannot be undone.`)) return;
    setBills((current) => current.filter((bill) => bill.id !== billId));
  }

  return (
    <section className="billing-page">
      <div className="billing-heading billing-heading-row">
        <div>
          <h1>Billing</h1>
          <p>Manage patient bills and payment status.</p>
        </div>
        <button className="billing-add-button" onClick={openAddForm}>
          <Plus size={18} /> Add Bill
        </button>
      </div>

      <div className="billing-summary">
        <div className="billing-summary-card">
          <div className="billing-summary-icon"><ReceiptText size={21} /></div>
          <p>Total Bills</p>
          <h2>{bills.length}</h2>
        </div>
        <div className="billing-summary-card">
          <div className="billing-summary-icon billing-paid-icon"><CheckCircle size={21} /></div>
          <p>Amount Collected</p>
          <h2>{formatAmount(paidAmount)}</h2>
        </div>
        <div className="billing-summary-card">
          <div className="billing-summary-icon billing-pending-icon"><Clock size={21} /></div>
          <p>Pending Amount</p>
          <h2>{formatAmount(pendingAmount)}</h2>
        </div>
      </div>

      <div className="billing-table-card">
        <div className="billing-toolbar">
          <div className="billing-search">
            <Search size={18} />
            <input
              type="search"
              placeholder="Search bill or patient..."
              value={search}
              onChange={(event) => setSearch(event.target.value)}
            />
          </div>
          <select
            aria-label="Filter by payment status"
            className="billing-filter"
            value={filter}
            onChange={(event) => setFilter(event.target.value as 'All' | BillStatus)}
          >
            <option value="All">All Payments</option>
            <option value="Paid">Paid</option>
            <option value="Pending">Pending</option>
          </select>
        </div>

        <div className="billing-table-scroll">
          <table className="billing-table">
            <thead>
              <tr>
                <th>Bill ID</th><th>Patient</th><th>Patient ID</th>
                <th>Date</th><th>Amount</th><th>Status</th><th>Actions</th>
              </tr>
            </thead>
            <tbody>
              {filteredBills.map((bill) => (
                <tr key={bill.id}>
                  <td>{bill.id}</td>
                  <td>{bill.patientName}</td>
                  <td>{bill.patientId}</td>
                  <td>{bill.date}</td>
                  <td>{formatAmount(bill.amount)}</td>
                  <td>
                    <span className={`billing-status ${bill.status === 'Paid' ? 'paid' : 'pending'}`}>
                      {bill.status}
                    </span>
                  </td>
                  <td>
                    <div className="billing-row-actions">
                      <button className="billing-edit-button" onClick={() => openEditForm(bill)}>
                        Edit
                      </button>
                      <button className="billing-delete-button" onClick={() => deleteBill(bill.id)}>
                        Delete
                      </button>
                    </div>
                  </td>
                </tr>
              ))}
              {filteredBills.length === 0 && (
                <tr><td colSpan={7} className="billing-empty">No bills found.</td></tr>
              )}
            </tbody>
          </table>
        </div>
        <div className="billing-table-footer">
          Showing {filteredBills.length} of {bills.length} bills
        </div>
      </div>

      {showForm && (
        <div className="billing-modal-backdrop">
          <div className="billing-modal" role="dialog" aria-modal="true" aria-labelledby="billing-form-title">
            <div className="billing-modal-heading">
              <h2 id="billing-form-title">{editingId ? 'Edit Bill' : 'Add New Bill'}</h2>
              <button className="billing-close-button" aria-label="Close form" onClick={() => setShowForm(false)}>
                <X size={20} />
              </button>
            </div>

            <form onSubmit={handleSubmit}>
              <label htmlFor="bill-patient-id">Patient ID</label>
              <input
                id="bill-patient-id"
                required
                placeholder="e.g. PAT-1001"
                value={form.patientId}
                onChange={(event) => setForm({ ...form, patientId: event.target.value })}
              />

              <label htmlFor="bill-patient-name">Patient Name</label>
              <input
                id="bill-patient-name"
                required
                placeholder="Enter patient name"
                value={form.patientName}
                onChange={(event) => setForm({ ...form, patientName: event.target.value })}
              />

              <div className="billing-form-row">
                <div>
                  <label htmlFor="bill-date">Date</label>
                  <input
                    id="bill-date"
                    type="date"
                    required
                    value={form.date}
                    onChange={(event) => setForm({ ...form, date: event.target.value })}
                  />
                </div>
                <div>
                  <label htmlFor="bill-amount">Amount (Rs.)</label>
                  <input
                    id="bill-amount"
                    type="number"
                    min="1"
                    step="1"
                    required
                    value={form.amount}
                    onChange={(event) => setForm({ ...form, amount: event.target.value })}
                  />
                </div>
              </div>

              <label htmlFor="bill-status">Payment Status</label>
              <select
                id="bill-status"
                value={form.status}
                onChange={(event) => setForm({ ...form, status: event.target.value as BillStatus })}
              >
                <option value="Pending">Pending</option>
                <option value="Paid">Paid</option>
              </select>

              {formError && <p className="billing-form-error">{formError}</p>}

              <div className="billing-form-actions">
                <button type="button" className="billing-cancel-button" onClick={() => setShowForm(false)}>
                  Cancel
                </button>
                <button type="submit" className="billing-add-button">
                  {editingId ? 'Save Changes' : 'Create Bill'}
                </button>
              </div>
            </form>
          </div>
        </div>
      )}
    </section>
  );
}
