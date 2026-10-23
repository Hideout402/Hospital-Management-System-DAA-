
import { useState } from 'react';
import { Save } from 'lucide-react';
import '../App.css';

export default function SettingsPage() {
  const [hospitalName, setHospitalName] = useState('CarePoint');
  const [phone, setPhone] = useState('');
  const [notifications, setNotifications] = useState(true);
  const [saved, setSaved] = useState(false);

  function handleSave() {
    setSaved(true);
  }

  return (
    <section className="settings-page">
      <div className="settings-heading">
        <div>
          <h1>Settings</h1>
          <p>Manage basic hospital preferences.</p>
        </div>
      </div>

      <div className="settings-card">
        <h2>Hospital Information</h2>

        <label htmlFor="hospitalName">Hospital Name</label>
        <input
          id="hospitalName"
          value={hospitalName}
          onChange={(e) => {
            setHospitalName(e.target.value);
            setSaved(false);
          }}
        />

        <label htmlFor="hospitalPhone">Contact Number</label>
        <input
          id="hospitalPhone"
          type="tel"
          placeholder="Enter contact number"
          value={phone}
          onChange={(e) => {
            setPhone(e.target.value);
            setSaved(false);
          }}
        />
      </div>

      <div className="settings-card settings-notifications">
        <div>
          <h2>Appointment Notifications</h2>
          <p>Enable appointment reminder notifications.</p>
        </div>

        <input
          aria-label="Appointment notifications"
          type="checkbox"
          checked={notifications}
          onChange={(e) => {
            setNotifications(e.target.checked);
            setSaved(false);
          }}
        />
      </div>

      <button className="settings-save-button" onClick={handleSave}>
        <Save size={18} />
        Save Settings
      </button>

      {saved && <p className="settings-success">Settings saved for this session.</p>}
    </section>
  );
}
