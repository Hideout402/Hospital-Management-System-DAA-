export type DoctorAvailability = 'Available' | 'On Leave' | 'In Surgery';

export interface Doctor {
  id: string;
  name: string;
  specialty: string;
  department: string;
  roomNumber: string;
  consultationFee: number; // In PKR
  availability: DoctorAvailability;
  phone?: string;
  email?: string;
  qualification?: string;
  experienceYears?: number;
  avatarBg?: string;
}

export type DoctorFormData = Omit<Doctor, 'id'> & {
  id?: string;
};
