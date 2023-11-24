import '../../../styles/sidebar/play/play.scss'
import { NavLink } from 'react-router-dom'
import Cup from '../../../assets/cup.png'
import { FaPlay } from "react-icons/fa";
import { FaUserFriends } from "react-icons/fa";
import { MdTableRestaurant } from "react-icons/md";
import { FaRobot } from "react-icons/fa";
import { FaBookMedical } from "react-icons/fa";
import { GiTrophyCup } from "react-icons/gi";
const play = () => {
    return (
        <div className='row vh-100 play'>
            <div className="play_select col-8">
                <NavLink to="/chess/play/playnow" className='play_item '>
                    <FaPlay className='fs-4 text-white' />
                    <button>Play now</button>
                </NavLink>
                <div className='play_item'>
                    <FaUserFriends className='fs-4 text-white' />
                    <button>play with friend</button>
                </div>
                <div className='play_item'>
                    <MdTableRestaurant className='fs-4 text-white' />
                    <button>Choose table</button>
                </div>
                <div className='play_item'>
                    <FaRobot className='fs-4 text-white' />
                    <button>play with computer</button>
                </div>
                <div className='play_cup'>
                    <div className='cup_item'>
                        <GiTrophyCup id='icon_cup' />
                        <span>BXH</span>
                    </div>
                    <div className='cup_item'>
                        <img src={Cup} alt="" />
                        <span id='cup_tex'>Tournament</span>
                    </div>
                    <div className='cup_item'>
                        <FaBookMedical id="icon_rules" />
                        <span>Rules</span>
                    </div>
                </div>
            </div>
            <div className='col-4'>
                Nhung ban be dang online
            </div>
        </div>
    )
}

export default play