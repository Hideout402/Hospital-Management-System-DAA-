
import { BrowserRouter, Routes, Route } from 'react-router-dom';
import MainLayout from './layouts/MainLayout';
import DashboardPage from './pages/DashboardPage';
import PatientPage from './pages/PatientPage';

function PlaceholderPage({ title }: { title: string }) {
  return (
    <section className="placeholder-page">
      <h2>{title}</h2>
      <p>This module will be developed by the assigned team member.</p>
    </section>
  );
}

export default function App() {
  return (
    <BrowserRouter>
      <Routes>
        <Route element={<MainLayout />}>
          <Route path="/" element={<DashboardPage />} />
          <Route path="/patients" element={<PatientPage  />} />
          <Route path="/doctors" element={<PlaceholderPage title="Doctors & Departments" />} />
          <Route path="/appointments" element={<PlaceholderPage title="Appointments" />} />
          <Route path="/emergency" element={<PlaceholderPage title="Emergency" />} />
          <Route path="/billing" element={<PlaceholderPage title="Billing" />} />
          <Route path="/settings" element={<PlaceholderPage title="Settings" />} />
        </Route>
      </Routes>
    </BrowserRouter>
  );
}
