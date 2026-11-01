import { useEffect, useMemo, useState } from 'react';
import { Search, ReceiptText, CheckCircle, Clock, Plus, X } from 'lucide-react';
import '../App.css';

type BillStatus = 'Unpaid' | 'Partially Paid' | 'Paid';

type BillLineItem = {
  description: string;
  quantity: number;
  unitPrice: number;
  amount: number;
};

type Bill = {
  id: number;
  patientId: number;
  patientName: string;
  lineItems: BillLineItem[];
  subtotal: number;
  discount: number;
  total: number;
  paidAmount: number;
  balance: number;
  status: BillStatus;
};

type BillForm = {
  patientId: string;
  patientName: string;
  description: string;
  quantity: string;
  unitPrice: string;
  discount: string;
  paidAmount: string;
};

const API_URL = 'http://localhost:8080/api/bills';

const emptyForm: BillForm = {
  patientId: '',
  patientName: '',
  description: 'Consultation',
  quantity: '1',
  unitPrice: '',
  discount: '0',
  paidAmount: '0',
};

const formatAmount = (amount: number) =>
  `Rs. ${amount.toLocaleString('en-PK', {
    minimumFractionDigits: 2,
    maximumFractionDigits: 2,
  })}`;

export default function BillingPage() {
  const [bills, setBills] = useState<Bill[]>([]);
  const [search, setSearch] = useState('');
  const [filter, setFilter] = useState<'All' | BillStatus>('All');
  const [showForm, setShowForm] = useState(false);
  const [editingId, setEditingId] = useState<number | null>(null);
  const [form, setForm] = useState<BillForm>(emptyForm);
  const [formError, setFormError] = useState('');
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState('');

  async function loadBills() {
    try {
      setLoading(true);
      setError('');

      const response = await fetch(API_URL);
      if (!response.ok) {
        throw new Error('Unable to load billing records.');
      }

      const data: Bill[] = await response.json();
      setBills(data);
    } catch {
      setError('Cannot connect to the billing backend. Please make sure the C++ server is running.');
    } finally {
      setLoading(false);
    }
  }

  useEffect(() => {
    loadBills();
  }, []);

  const filteredBills = useMemo(() => {
    const query = search.trim().toLowerCase();

    return bills.filter((bill) => {
      const patientText = `${bill.patientId} ${bill.patientName}`.toLowerCase();
      const matchesSearch =
        patientText.includes(query) ||
        String(bill.id).toLowerCase().includes(query);

      return matchesSearch && (filter === 'All' || bill.status === filter);
    });
  }, [bills, search, filter]);

  const collectedAmount = bills.reduce((sum, bill) => sum + bill.paidAmount, 0);
  const pendingAmount = bills.reduce((sum, bill) => sum + Math.max(0, bill.balance), 0);

  function openAddForm() {
    setEditingId(null);
    setForm({ ...emptyForm, patientId: '', patientName: '', description: 'Consultation' });
    setFormError('');
    setShowForm(true);
  }

  function openEditForm(bill: Bill) {
    const firstItem = bill.lineItems[0];

    setEditingId(bill.id);
    setForm({
      patientId: String(bill.patientId),
      patientName: bill.patientName,
      description: firstItem?.description ?? 'Consultation',
      quantity: String(firstItem?.quantity ?? 1),
      unitPrice: String(firstItem?.unitPrice ?? 0),
      discount: String(bill.discount),
      paidAmount: String(bill.paidAmount),
    });
    setFormError('');
    setShowForm(true);
  }

  async function handleSubmit(event: React.FormEvent<HTMLFormElement>) {
    event.preventDefault();

    const patientId = Number(form.patientId);
    const patientName = form.patientName.trim();
    const description = form.description.trim();
    const quantity = Number(form.quantity);
    const unitPrice = Number(form.unitPrice);
    const discount = Number(form.discount);
    const paidAmount = Number(form.paidAmount);

    if (!Number.isInteger(patientId) || patientId <= 0) {
      setFormError('Please enter a valid patient ID.');
      return;
    }

    if (!patientName) {
      setFormError('Please enter the patient name.');
      return;
    }

    if (!description) {
      setFormError('Please enter a service description.');
      return;
    }

    if (!Number.isFinite(quantity) || quantity <= 0 || !Number.isInteger(quantity)) {
      setFormError('Quantity must be a positive whole number.');
      return;
    }

    if (!Number.isFinite(unitPrice) || unitPrice < 0) {
      setFormError('Unit price cannot be negative.');
      return;
    }

    if (!Number.isFinite(discount) || discount < 0) {
      setFormError('Discount cannot be negative.');
      return;
    }

    if (!Number.isFinite(paidAmount) || paidAmount < 0) {
      setFormError('Paid amount cannot be negative.');
      return;
    }

    const payload = {
      patientId,
      patientName,
      discount,
      paidAmount,
      lineItems: [
        {
          description,
          quantity,
          unitPrice,
        },
      ],
    };

    try {
      const response = await fetch(editingId !== null ? `${API_URL}/${editingId}` : API_URL, {
        method: editingId !== null ? 'PUT' : 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(payload),
      });

      const result = await response.json();
      if (!response.ok) {
        throw new Error(result.message || 'Bill operation failed.');
      }

      await loadBills();
      setShowForm(false);
      setForm({ ...emptyForm });
      setFormError('');
      setEditingId(null);
    } catch (err) {
      setFormError(err instanceof Error ? err.message : 'Something went wrong while saving the bill.');
    }
  }

  async function deleteBill(billId: number) {
    if (!window.confirm(`Delete bill #${billId}? This cannot be undone.`)) {
      return;
    }

    try {
      const response = await fetch(`${API_URL}/${billId}`, { method: 'DELETE' });
      const result = await response.json();

      if (!response.ok) {
        throw new Error(result.message || 'Bill could not be deleted.');
      }

      await loadBills();
    } catch (err) {
      window.alert(err instanceof Error ? err.message : 'Something went wrong.');
    }
  }

  async function recordPayment(bill: Bill) {
    const rawAmount = window.prompt(
      `Enter payment amount for bill #${bill.id}. Outstanding balance: ${formatAmount(Math.max(0, bill.balance))}`,
      String(Math.max(0, bill.balance))
    );

    if (rawAmount === null) {
      return;
    }

    const amount = Number(rawAmount);
    if (!Number.isFinite(amount) || amount <= 0) {
      window.alert('Payment must be a positive number.');
      return;
    }

    try {
      const response = await fetch(`${API_URL}/${bill.id}/payment`, {
        method: 'PATCH',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ amount }),
      });

      const result = await response.json();
      if (!response.ok) {
        throw new Error(result.message || 'Payment could not be recorded.');
      }

      await loadBills();
    } catch (err) {
      window.alert(err instanceof Error ? err.message : 'Payment failed.');
    }
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
          <h2>{formatAmount(collectedAmount)}</h2>
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
            <option value="Partially Paid">Partially Paid</option>
            <option value="Unpaid">Unpaid</option>
          </select>
        </div>

        {loading && <p>Loading billing records...</p>}
        {error && <p role="alert">{error}</p>}

        {!loading && !error && (
          <>
            <div className="billing-table-scroll">
              <table className="billing-table">
                <thead>
                  <tr>
                    <th>Bill ID</th>
                    <th>Patient</th>
                    <th>Patient ID</th>
                    <th>Subtotal</th>
                    <th>Total</th>
                    <th>Balance</th>
                    <th>Status</th>
                    <th>Actions</th>
                  </tr>
                </thead>
                <tbody>
                  {filteredBills.map((bill) => (
                    <tr key={bill.id}>
                      <td>{bill.id}</td>
                      <td>{bill.patientName}</td>
                      <td>{bill.patientId}</td>
                      <td>{formatAmount(bill.subtotal)}</td>
                      <td>{formatAmount(bill.total)}</td>
                      <td>{formatAmount(bill.balance)}</td>
                      <td>
                        <span className={`billing-status ${bill.status.toLowerCase().replace(/\s+/g, '-')}`}>
                          {bill.status}
                        </span>
                      </td>
                      <td>
                        <div className="billing-row-actions">
                          <button className="billing-edit-button" onClick={() => openEditForm(bill)}>
                            Edit
                          </button>
                          {bill.balance > 0 && (
                            <button className="billing-edit-button" onClick={() => recordPayment(bill)}>
                              Pay
                            </button>
                          )}
                          <button className="billing-delete-button" onClick={() => deleteBill(bill.id)}>
                            Delete
                          </button>
                        </div>
                      </td>
                    </tr>
                  ))}
                  {filteredBills.length === 0 && (
                    <tr>
                      <td colSpan={8} className="billing-empty">No bills found.</td>
                    </tr>
                  )}
                </tbody>
              </table>
            </div>
            <div className="billing-table-footer">
              Showing {filteredBills.length} of {bills.length} bills
            </div>
          </>
        )}
      </div>

      {showForm && (
        <div className="billing-modal-backdrop">
          <div className="billing-modal" role="dialog" aria-modal="true" aria-labelledby="billing-form-title">
            <div className="billing-modal-heading">
              <h2 id="billing-form-title">{editingId !== null ? 'Edit Bill' : 'Add New Bill'}</h2>
              <button className="billing-close-button" aria-label="Close form" onClick={() => setShowForm(false)}>
                <X size={20} />
              </button>
            </div>

            <form onSubmit={handleSubmit}>
              <label htmlFor="bill-patient-id">Patient ID</label>
              <input
                id="bill-patient-id"
                required
                type="number"
                min="1"
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

              <label htmlFor="bill-description">Service Description</label>
              <input
                id="bill-description"
                required
                placeholder="Consultation"
                value={form.description}
                onChange={(event) => setForm({ ...form, description: event.target.value })}
              />

              <div className="billing-form-row">
                <div>
                  <label htmlFor="bill-quantity">Quantity</label>
                  <input
                    id="bill-quantity"
                    type="number"
                    min="1"
                    step="1"
                    required
                    value={form.quantity}
                    onChange={(event) => setForm({ ...form, quantity: event.target.value })}
                  />
                </div>
                <div>
                  <label htmlFor="bill-unit-price">Unit Price (Rs.)</label>
                  <input
                    id="bill-unit-price"
                    type="number"
                    min="0"
                    step="0.01"
                    required
                    value={form.unitPrice}
                    onChange={(event) => setForm({ ...form, unitPrice: event.target.value })}
                  />
                </div>
              </div>

              <div className="billing-form-row">
                <div>
                  <label htmlFor="bill-discount">Discount (Rs.)</label>
                  <input
                    id="bill-discount"
                    type="number"
                    min="0"
                    step="0.01"
                    value={form.discount}
                    onChange={(event) => setForm({ ...form, discount: event.target.value })}
                  />
                </div>
                <div>
                  <label htmlFor="bill-paid-amount">Paid Amount (Rs.)</label>
                  <input
                    id="bill-paid-amount"
                    type="number"
                    min="0"
                    step="0.01"
                    value={form.paidAmount}
                    onChange={(event) => setForm({ ...form, paidAmount: event.target.value })}
                  />
                </div>
              </div>

              {formError && <p className="billing-form-error">{formError}</p>}

              <div className="billing-form-actions">
                <button type="button" className="billing-cancel-button" onClick={() => setShowForm(false)}>
                  Cancel
                </button>
                <button type="submit" className="billing-add-button">
                  {editingId !== null ? 'Save Changes' : 'Create Bill'}
                </button>
              </div>
            </form>
          </div>
        </div>
      )}
    </section>
  );
}
