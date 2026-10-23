
export type BillStatus = 'Paid' | 'Pending';

export type Bill = {
  id: string;
  patientId: string;
  patientName: string;
  date: string;
  amount: number;
  status: BillStatus;
};

export const initialBills: Bill[] = [
  {
    id: 'BILL-1001',
    patientId: 'PAT-1001',
    patientName: 'Ayesha Khan',
    date: '2026-10-20',
    amount: 8500,
    status: 'Paid',
  },
  {
    id: 'BILL-1002',
    patientId: 'PAT-1002',
    patientName: 'Hamza Malik',
    date: '2026-10-21',
    amount: 12000,
    status: 'Pending',
  },
  {
    id: 'BILL-1003',
    patientId: 'PAT-1003',
    patientName: 'Fatima Iqbal',
    date: '2026-10-21',
    amount: 6500,
    status: 'Paid',
  },
  {
    id: 'BILL-1004',
    patientId: 'PAT-1004',
    patientName: 'Ali Raza',
    date: '2026-10-22',
    amount: 15000,
    status: 'Pending',
  },
  {
    id: 'BILL-1005',
    patientId: 'PAT-1005',
    patientName: 'Hira Ahmed',
    date: '2026-10-22',
    amount: 4500,
    status: 'Paid',
  },
];
