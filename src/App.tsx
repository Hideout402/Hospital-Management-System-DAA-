
import { BrowserRouter, Routes, Route } from 'react-router-dom';
import MainLayout from './layouts/MainLayout';
import DashboardPage from './pages/DashboardPage';
import PatientPage from './pages/PatientPage';
import BillingPage from './pages/BillingPage';
import SettingsPage from './pages/SettingsPage';
import EmergencyPage from './pages/EmergencyPage';
import NavigationPage from './pages/NavigationPage';
import AppointmentsPageEntry from './pages/AppointmentsPage';
import DoctorDirectory from './pages/DoctorDirectory';


export default function App() {
  return (
    <BrowserRouter>
      <Routes>
        <Route element={<MainLayout />}>
          <Route path="/" element={<DashboardPage />} />
          <Route path="/patients" element={<PatientPage />} />
          <Route path="/doctors" element={<DoctorDirectory />} />
          <Route path="/appointments/*" element={<AppointmentsPageEntry />} />
          <Route path="/emergency" element={<EmergencyPage />} />
          <Route path="/navigation" element={<NavigationPage />} />
          <Route path="/billing" element={<BillingPage />} />
          <Route path="/settings" element={<SettingsPage />} />
        </Route>
      </Routes>
    </BrowserRouter>
  );
}
