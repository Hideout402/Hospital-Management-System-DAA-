
export type Patient = {
  id: number;
  name: string;
  age: number;
  gender: string;
  phone: string;
  department: string;
  status: 'Active' | 'Discharged';
  address?: string;
  visitHistory?: string[];
};

export const initialPatients: Patient[] = [
  {
    id: 1001,
    name: 'Ayesha Khan',
    age: 28,
    gender: 'Female',
    phone: '0301-1234567',
    department: 'Cardiology',
    status: 'Active',
  },
  {
    id: 1002,
    name: 'Hamza Malik',
    age: 42,
    gender: 'Male',
    phone: '0302-2345678',
    department: 'Neurology',
    status: 'Active',
  },
  {
    id: 1003,
    name: 'Fatima Iqbal',
    age: 35,
    gender: 'Female',
    phone: '0303-3456789',
    department: 'Orthopedics',
    status: 'Discharged',
  },
  {
    id: 1004,
    name: 'Ali Raza',
    age: 51,
    gender: 'Male',
    phone: '0304-4567890',
    department: 'General Medicine',
    status: 'Active',
  },
  {
    id: 1005,
    name: 'Hira Ahmed',
    age: 23,
    gender: 'Female',
    phone: '0305-5678901',
    department: 'Dermatology',
    status: 'Discharged',
  },
];
