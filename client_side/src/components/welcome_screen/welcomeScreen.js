import '../../styles/welcomeScreen/welcomeScreen.scss'
import { NavLink } from 'react-router-dom'
import Logo from '../../assets/logo.png'
import Banner1 from '../../assets/banner1.jpg'
import Banner2 from '../../assets/banner2.jpg'
import Banner3 from '../../assets/banner3.jpg'
import Banner4 from '../../assets/banner4.jpg'
import Banner5 from '../../assets/banner5.jpg'
const welcomeScreen = () => {
    return (
        <div className="welcomeScreenContainer container-fluid">
            <div className='welcomeScreenBanner container-fluid'>
                <div id="demo" class="carousel slide" data-bs-ride="carousel">
                    <div className="carousel-indicators">
                        <button type="button" data-bs-target="#demo" data-bs-slide-to="0" className="active"></button>
                        <button type="button" data-bs-target="#demo" data-bs-slide-to="1"></button>
                        <button type="button" data-bs-target="#demo" data-bs-slide-to="2"></button>
                        <button type="button" data-bs-target="#demo" data-bs-slide-to="3"></button>
                        <button type="button" data-bs-target="#demo" data-bs-slide-to="4"></button>
                    </div>
                    <div className="carousel-inner">
                        <div className="carousel-item active">
                            <img src={Banner2} alt="Los Angeles" className="d-block w-100" />
                        </div>
                        <div className="carousel-item">
                            <img src={Banner1} alt="Chicago" className="d-block w-100" />
                        </div>
                        <div className="carousel-item">
                            <img src={Banner3} alt="New York" className="d-block w-100" />
                        </div>
                        <div className="carousel-item">
                            <img src={Banner4} alt="New York" className="d-block w-100" />
                        </div>
                        <div className="carousel-item">
                            <img src={Banner5} alt="New York" className="d-block w-100" />
                        </div>
                    </div>
                    <button className="carousel-control-prev" type="button" data-bs-target="#demo" data-bs-slide="prev">
                        <span className="carousel-control-prev-icon"></span>
                    </button>
                    <button className="carousel-control-next" type="button" data-bs-target="#demo" data-bs-slide="next">
                        <span className="carousel-control-next-icon"></span>
                    </button>
                </div>
            </div>
            <div className='welcomeScreenNavbar container-fluid'>
                <div className='row'>
                    <div className='col-1'>
                        <img src={Logo} alt='logo' className='logo mt-2' />
                    </div>
                    <span className='appName col-4'>RoyalChess</span>
                    <div className='login_signup col-7' >
                        <NavLink className="login" to="/login">Log in</NavLink>
                        <NavLink className="signup" to="/signup">Sign up</NavLink>
                    </div>
                </div>
            </div>
        </div>
    )
}
export default welcomeScreen