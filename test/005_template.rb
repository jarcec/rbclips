require 'test/unit'
require 'rbclips'

class Test_Template < Test::Unit::TestCase
  def test_exists
    assert Clips.constants.member?(:Template)
  end

  def test_new_hash
    assert_nothing_raised               { Clips::Template.new :name => 'human', :slots => [:name, 'age'] }
    assert_nothing_raised               { Clips::Template.new :name => 'human', :slots => %w(name age) }
    assert_nothing_raised               { Clips::Template.new :name => 'human', :slots => {:name => {:multislot => false}, 'age' => {:default => 30}} }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :type => [:any] }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 3 }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 'human' }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :slots => [:name, :age] }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 'human', :slots => [2, 1, 4] }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 'human', :slots => { 2 => {:default => 30}} }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 'human', :slots => { :name => {:pico => 30}} }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 'human', :slots => { :name => {'pico' => 30}} }
    assert_raise(Clips::ArgumentError)  { Clips::Template.new :name => 'human', :slots => { :name => {2 => 30}} }

    a = Clips::Template.new :name => 'human', :slots => [:name, 'age']
    assert_equal a.instance_eval { @name }, 'human'
    assert_equal a.instance_eval { @slots }, { :name => nil, :age => nil }

    a = Clips::Template.new :name => 'human', :slots => %w(name age)
    assert_equal a.instance_eval { @name }, 'human'
    assert_equal a.instance_eval { @slots }, { :name => nil, :age => nil }

    a = Clips::Template.new :name => 'human', :slots => {:name => { :multislot => false}, :age => { :default => 30}}
    assert_equal a.instance_eval { @name }, 'human'
    assert_equal a.instance_eval { @slots }, { :name => {:multislot => false}, :age => {:default => 30} }
  end
end
