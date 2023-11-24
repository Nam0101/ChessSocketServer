import { Outlet } from 'react-router-dom'
import { NavLink } from 'react-router-dom'
import '../../styles/sidebar/sidebar.scss'
import Logo from '../../assets/logo.png'
import { IoMdHome } from "react-icons/io";
import { IoGameController } from "react-icons/io5";
import { FaUserFriends } from "react-icons/fa";
import { GiWorld } from "react-icons/gi";
import { FaHistory } from "react-icons/fa";
import { IoLogoWechat } from "react-icons/io5";
import { IoSettings } from "react-icons/io5";

const sidebar = () => {
    return (
        <div className="sidebar container-fluid vh-100">
            <div className='sidebar_content'>
                <div className='sidebar_logo'>
                    <img src={Logo} alt='logo' />
                    <span>RoyalChess</span>
                </div>
                <div className='sidebar_container'>
                    <NavLink to="/chess" className='sidebar_item' end>
                        <IoMdHome className='fs-3 text-white' />
                        <span>Home</span>
                    </NavLink>
                    <NavLink to="/chess/play" className='sidebar_item' >
                        <IoGameController className='fs-4 text-white' />
                        <span>Play</span>
                    </NavLink>
                    <NavLink to="/chess/friends" className='sidebar_item' >
                        <FaUserFriends className='fs-4 text-white' />
                        <span>Friends</span>
                    </NavLink>
                    <NavLink to="/chess/chat" className='sidebar_item'>
                        <IoLogoWechat className='fs-3 text-white' />
                        <span>Chat</span>
                    </NavLink>
                    <NavLink to="/chess/history" className='sidebar_item' >
                        <FaHistory className='fs-4 text-white' />
                        <span>History</span>
                    </NavLink>
                    <NavLink to="/chess/leaderboard" className='sidebar_item' >
                        <GiWorld className='fs-4 text-white' />
                        <span>Leaderboard</span>
                    </NavLink>
                    <NavLink to="/chess/setting" className='sidebar_item' >
                        <IoSettings className='fs-4 text-white' />
                        <span>Settings</span>
                    </NavLink>
                </div>
            </div>
            <div className='outlet'>
                <Outlet />
            </div>
        </div>
    )
}

export default sidebar