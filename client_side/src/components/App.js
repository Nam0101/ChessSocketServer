import { Routes, Route } from "react-router-dom";
import '../styles/App.scss';
import WelcomeScreen from './welcome_screen/welcomeScreen';
import Login from './logIn_signUp/login';
import Signup from './logIn_signUp/Signup';
import Sidebar from './sidebar/sidebar';
import Home from './sidebar/home/home';
import Play from './sidebar/play/play';
import Friends from './sidebar/friends/friends';
import Chat from './sidebar/chat/chat';
import History from './sidebar/history/history';
import Leaderboard from './sidebar/leaderboard/leaderboard';
import Settings from './sidebar/setting/setting';
import PlayNow from "./sidebar/play/playNow";

function App() {
  return (
    <div className="App">
      <Routes>
        <Route path="/" element={<WelcomeScreen />} />
        <Route path="/login" element={<Login />} />
        <Route path="/signup" element={<Signup />} />
        <Route path="/chess" element={<Sidebar />} >
          <Route index element={<Home />} />
          <Route path="/chess/play" element={<Play />} />
          <Route path="/chess/play/playnow" element={<PlayNow />} />
          <Route path="/chess/friends" element={<Friends />} />
          <Route path="/chess/chat" element={<Chat />} />
          <Route path="/chess/history" element={<History />} />
          <Route path="/chess/leaderboard" element={<Leaderboard />} />
          <Route path="/chess/setting" element={<Settings />} />
        </Route>
      </Routes>
    </div>
  );
}

export default App;
