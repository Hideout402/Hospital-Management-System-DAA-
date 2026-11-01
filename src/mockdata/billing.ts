export type BillStatus = 'Unpaid' | 'Partially Paid' | 'Paid';

export type BillLineItem = {
  description: string;
  quantity: number;
  unitPrice: number;
  amount: number;
};

export type Bill = {
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

export const initialBills: Bill[] = [
  {
    id: 1,
    patientId: 1001,
    patientName: 'Ayesha Khan',
    lineItems: [
      { description: 'Consultation', quantity: 1, unitPrice: 2500, amount: 2500 },
      { description: 'Diagnostic Lab', quantity: 1, unitPrice: 1200, amount: 1200 },
    ],
    subtotal: 3700,
    discount: 300,
    total: 3400,
    paidAmount: 2500,
    balance: 900,
    status: 'Partially Paid',
  },
  {
    id: 2,
    patientId: 1002,
    patientName: 'Hamza Malik',
    lineItems: [
      { description: 'Neurology Consultation', quantity: 1, unitPrice: 3200, amount: 3200 },
      { description: 'MRI Scan', quantity: 1, unitPrice: 5600, amount: 5600 },
    ],
    subtotal: 8800,
    discount: 500,
    total: 8300,
    paidAmount: 8300,
    balance: 0,
    status: 'Paid',
  },
];
